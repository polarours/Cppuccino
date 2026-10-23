#include "server.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace key_value_store {

Server::Server(const std::string& host, int port, const std::string& persistencePath)
    : host_(host)
    , port_(port)
    , server_fd_(-1)
    , storeManager_(persistencePath)
    , protocol_(storeManager_.store())
    , running_(false) {
    storeManager_.load();
}

Server::~Server() {
    stop();
}

void Server::start() {
    int fd = setupSocket();

    try {
        // The flag goes up before the socket is published, so that a stop()
        // that runs in between sees the flag down and the accept loop below
        // never starts.
        running_ = true;

        // The socket is published under the lock, so that stop() can shut it
        // down. It stays owned by this function: this is the only place
        // that closes it.
        {
            std::lock_guard<std::mutex> lock(mutex_);
            server_fd_ = fd;
        }

        std::cout << "key-value-store server started on " << host_ << ":" << port_ << "\n";
        std::cout << "Persistence: " << storeManager_.persistencePath() << "\n";
        std::cout << "Type 'help' for available commands\n\n";

        while (running_) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);

            int client_fd = accept(fd, (struct sockaddr*)&clientAddr, &clientLen);
            if (client_fd < 0) {
                if (!running_) break;
                // The socket is gone. Retrying would spin on the error.
                if (errno == EBADF || errno == EINVAL || errno == ENOTSOCK) break;
                continue;
            }

            std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << "\n";

            // The running flag and the thread list are read under the same
            // lock. A client that is accepted at the very moment the server
            // is stopped would otherwise end up in a list that nobody joins.
            bool spawned = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (running_) {
                    clientThreads_.emplace_back(&Server::handleClient, this, client_fd);
                    spawned = true;
                }
            }
            if (!spawned) {
                close(client_fd);
                break;
            }
        }
    } catch (...) {
        // The socket is forgotten before it is closed, so that a stop() that
        // runs at the same time cannot shut down a descriptor that the
        // system has already handed out to somebody else.
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (server_fd_ == fd) server_fd_ = -1;
        }
        close(fd);
        throw;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (server_fd_ == fd) server_fd_ = -1;
    }
    close(fd);
}

void Server::stop() {
    running_ = false;

    // shutdown() before close(): on Linux, close() does not wake a thread
    // that is already blocked in accept() or in read(), so the accept loop
    // would never observe running_ == false and the join below would hang
    // forever. The socket itself has to be shut down.
    //
    // The shutdown happens under the lock. A descriptor that is closed while
    // another thread still holds its number may already belong to somebody
    // else by then.
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (server_fd_ >= 0) {
            shutdown(server_fd_, SHUT_RDWR);
        }
        for (int fd : clientFds_) {
            shutdown(fd, SHUT_RDWR);
        }
    }

    // The client threads are taken out of the list under the lock and joined
    // outside of it.
    for (;;) {
        std::vector<std::thread> threads;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (clientThreads_.empty()) break;
            threads.swap(clientThreads_);
        }
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    storeManager_.save();
    std::cout << "Server stopped\n";
}

bool Server::isRunning() const {
    return running_;
}

void Server::handleClient(int client_fd) {
    // Register the socket so stop() can wake this thread if it is blocked.
    // The check and the registration are one step, so that a connection
    // that arrives while the server is being stopped is closed instead of
    // being left to block in read() forever.
    bool registered = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) {
            clientFds_.insert(client_fd);
            registered = true;
        }
    }
    if (!registered) {
        close(client_fd);
        return;
    }

    char buffer[4096];

    while (running_) {
        ssize_t bytesRead = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            break;
        }

        buffer[bytesRead] = '\0';
        std::string request(buffer);

        std::istringstream stream(request);
        std::string line;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (line.empty()) continue;

            ProtocolResponse response = protocol_.execute(line);

            if (response.status == ProtocolStatus::bye) {
                std::string serialized = response.serialize();
                write(client_fd, serialized.c_str(), serialized.size());
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    clientFds_.erase(client_fd);
                }
                close(client_fd);
                return;
            }

            if (response.status == ProtocolStatus::ok && storeManager_.autoPersist()) {
                storeManager_.save();
            }

            std::string serialized = response.serialize();
            write(client_fd, serialized.c_str(), serialized.size());
        }
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        clientFds_.erase(client_fd);
    }
    close(client_fd);
}

int Server::setupSocket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        close(fd);
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);

    if (host_ == "0.0.0.0" || host_ == "*") {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) <= 0) {
            close(fd);
            throw std::runtime_error("Invalid address: " + host_);
        }
    }

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        throw std::runtime_error("Failed to bind to " + host_ + ":" + std::to_string(port_));
    }

    if (listen(fd, 10) < 0) {
        close(fd);
        throw std::runtime_error("Failed to listen");
    }

    return fd;
}

} // namespace key_value_store
