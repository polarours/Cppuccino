#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#include "server.hpp"

namespace tiny_http_server {

Server::Server(const std::string& addr, int port)
    : addr_(addr)
    , port_(port)
    , server_fd_(-1) 
    {

    }

Server::~Server() {
    cleanup();
}

void Server::start() {
    setup_socket();

    std::cout << "tiny-http-server started on " << addr_ << ":" << port_ << std::endl;

    while(true) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }
        handle_client(client_fd);
        close(client_fd);
    }
}

void Server::setup_socket() { 
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd_, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd_, 10) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::handle_client(int client_fd) {
    char buffer[4096];

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        std::cerr << "Failed to read from client" << std::endl;
        close(client_fd);
        return;
    }

    buffer[bytes_read] = '\0';
    std::cout << "Received request:\n" << buffer << std::endl;

    const char* response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 13\r\n"
                           "\r\n"
                           "Hello, World!";

    write(client_fd, response, strlen(response));
}

void Server::cleanup() {
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

}