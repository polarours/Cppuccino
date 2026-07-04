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
    setup_socket();
    running_ = true;

    std::cout << "tiny-http-server started on " << addr_ << ":" << port_ << std::endl;

    while (running_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (!running_) {
                break;
            }
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }

        thread_pool_.enqueue([this, client_fd]() {
            handle_client(client_fd);
            close(client_fd);
        });
    }

    std::cout << "tiny-http-server stopped" << std::endl;
}

void Server::stop() {
    running_ = false;
    cleanup();
}

void Server::addRoute(const std::string& method, const std::string& path, RouteHandler handler) {
    router_.addRoute(method, path, std::move(handler));
}

void Server::setup_socket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int reuse_addr = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0) {
        cleanup();
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, addr_.c_str(), &server_addr.sin_addr) != 1) {
        cleanup();
        throw std::runtime_error("Invalid IPv4 address: " + addr_);
    }

    if (bind(server_fd_, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::string message = "Failed to bind socket";
        cleanup();
        throw std::runtime_error(message);
    }

    if (listen(server_fd_, 10) < 0) {
        cleanup();
        throw std::runtime_error("Failed to listen on socket");
    }
}

void Server::handle_client(int client_fd) {
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

void Server::cleanup() {
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

} // namespace tiny_http_server
