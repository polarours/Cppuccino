#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "http/request.hpp"
#include "http/response.hpp"
#include "server.hpp"

namespace tiny_http_server {

Server::Server(const std::string& addr, int port, std::size_t thread_count)
    : addr_(addr)
    , port_(port)
    , server_fd_(-1)
    , thread_pool_(thread_count)
    , running_(false) {}

Server::~Server() {
    stop();
}

void Server::start() {
    int fd = setup_socket();

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

        std::cout << "tiny-http-server started on " << addr_ << ":" << port_ << std::endl;

        while (running_) {
            int client_fd = accept(fd, nullptr, nullptr);
            if (client_fd < 0) {
                if (errno == EINTR) {
                    continue;
                }
                if (!running_) {
                    break;
                }
                // The socket is gone. Retrying would spin on the error.
                if (errno == EBADF || errno == EINVAL || errno == ENOTSOCK) {
                    break;
                }
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }

            // The running flag is read under the lock, so that a connection
            // that arrives while the server is being stopped is closed
            // instead of being handed to a pool that is about to stop.
            bool accepted = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                accepted = running_;
            }
            if (!accepted) {
                close(client_fd);
                break;
            }

            thread_pool_.enqueue([this, client_fd]() {
                handle_client(client_fd);
                close(client_fd);
            });
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

    std::cout << "tiny-http-server stopped" << std::endl;
}

void Server::stop() {
    running_ = false;

    // shutdown() before close(): on Linux, close() does not wake a thread
    // that is already blocked in accept() or in read(), so the accept loop
    // would never observe running_ == false and the pool would hang on its
    // join. The sockets themselves have to be shut down.
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
}

void Server::addRoute(const std::string& method, const std::string& path, RouteHandler handler) {
    router_.addRoute(method, path, std::move(handler));
}

int Server::setup_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int reuse_addr = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0) {
        close(fd);
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, addr_.c_str(), &server_addr.sin_addr) != 1) {
        close(fd);
        throw std::runtime_error("Invalid IPv4 address: " + addr_);
    }

    if (bind(fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::string message = "Failed to bind socket";
        close(fd);
        throw std::runtime_error(message);
    }

    if (listen(fd, 10) < 0) {
        close(fd);
        throw std::runtime_error("Failed to listen on socket");
    }

    return fd;
}

void Server::handle_client(int client_fd) {
    // Register the socket so stop() can wake this worker if it is blocked
    // in read(). The check and the registration are one step, so that a
    // connection that arrives while the server is being stopped is closed
    // instead of being left to block forever.
    bool registered = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) {
            clientFds_.insert(client_fd);
            registered = true;
        }
    }
    if (!registered) {
        return;
    }

    // The socket is taken out of the registry on every way out, so that
    // stop() does not shut down a descriptor that is already gone.
    struct Unregister {
        Server* server;
        int fd;
        ~Unregister() {
            std::lock_guard<std::mutex> lock(server->mutex_);
            server->clientFds_.erase(fd);
        }
    } unregister{this, client_fd};

    char buffer[4096];

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        std::cerr << "Failed to read from client" << std::endl;
        return;
    }

    buffer[bytes_read] = '\0';
    const std::string raw_request(buffer, static_cast<std::size_t>(bytes_read));
    HttpRequest request;

    if (!request.parse(raw_request)) {
        HttpResponse bad_request_response;
        bad_request_response.set_status(400, "Bad Request");
        bad_request_response.set_body("Bad Request");
        send_response(client_fd, bad_request_response);
        return;
    }

    std::cout << request.method() << " " << request.path()
              << " " << request.version() << std::endl;

    const HttpResponse response = router_.route(request);
    send_response(client_fd, response);
}

void Server::send_response(int client_fd, const HttpResponse& response) {
    const std::string serialized = response.to_string();
    std::size_t total_sent = 0;

    while (total_sent < serialized.size()) {
        const ssize_t bytes_sent = write(
            client_fd,
            serialized.data() + total_sent,
            serialized.size() - total_sent
        );

        if (bytes_sent <= 0) {
            std::cerr << "Failed to write response" << std::endl;
            return;
        }

        total_sent += static_cast<std::size_t>(bytes_sent);
    }
}

} // namespace tiny_http_server
