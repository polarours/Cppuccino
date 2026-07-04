#include "server.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
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
    setupSocket();
    running_ = true;

    std::cout << "key-value-store server started on " << host_ << ":" << port_ << "\n";
    std::cout << "Persistence: " << storeManager_.persistencePath() << "\n";
    std::cout << "Type 'help' for available commands\n\n";

    while (running_) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int client_fd = accept(server_fd_, (struct sockaddr*)&clientAddr, &clientLen);
        if (client_fd < 0) {
            if (running_) {
                std::cerr << "Failed to accept connection\n";
            }
            continue;
        }

        std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << "\n";

        clientThreads_.emplace_back(&Server::handleClient, this, client_fd);
    }
}

void Server::stop() {
    running_ = false;

    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }

    for (auto& thread : clientThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    clientThreads_.clear();

    storeManager_.save();
    std::cout << "Server stopped\n";
}

bool Server::isRunning() const {
    return running_;
}

void Server::handleClient(int client_fd) {
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

    close(client_fd);
}

void Server::setupSocket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int reuse = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        close(server_fd_);
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
            close(server_fd_);
            throw std::runtime_error("Invalid address: " + host_);
        }
    }

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to bind to " + host_ + ":" + std::to_string(port_));
    }

    if (listen(server_fd_, 10) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to listen");
    }
}

void Server::cleanup() {
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

} // namespace key_value_store
