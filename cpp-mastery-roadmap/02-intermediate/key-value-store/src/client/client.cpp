#include "client.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace key_value_store {

Client::Client(const std::string& host, int port)
    : host_(host)
    , port_(port)
    , sock_(-1)
    , connected_(false)
    , echo_(true) {}

Client::~Client() {
    disconnect();
}

bool Client::connect() {
    if (connected_) {
        return true;
    }

    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        std::cerr << "Failed to create socket\n";
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << host_ << "\n";
        close(sock_);
        sock_ = -1;
        return false;
    }

    if (::connect(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to connect to " << host_ << ":" << port_ << "\n";
        close(sock_);
        sock_ = -1;
        return false;
    }

    connected_ = true;
    if (echo_) {
        std::cout << "Connected to " << host_ << ":" << port_ << "\n";
    }
    return true;
}

void Client::disconnect() {
    if (sock_ >= 0) {
        close(sock_);
        sock_ = -1;
    }
    connected_ = false;
}

bool Client::isConnected() const {
    return connected_;
}

ProtocolResponse Client::execute(const std::string& command) {
    if (!ensureConnected()) {
        return {ProtocolStatus::error, "not connected"};
    }

    std::string request = command + "\n";
    ssize_t bytesSent = ::send(sock_, request.c_str(), request.size(), 0);
    if (bytesSent <= 0) {
        connected_ = false;
        return {ProtocolStatus::error, "send failed"};
    }

    char buffer[4096];
    ssize_t bytesRead = recv(sock_, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        connected_ = false;
        return {ProtocolStatus::error, "recv failed"};
    }

    buffer[bytesRead] = '\0';
    std::string response(buffer);

    if (!response.empty() && response.back() == '\n') {
        response.pop_back();
    }

    return ProtocolResponse::parse(response);
}

void Client::interactive() {
    std::cout << "key-value-store client\n";
    std::cout << "Type 'help' for available commands, 'quit' to exit\n\n";

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }

        if (line == "quit" || line == "exit") {
            execute("QUIT");
            break;
        }

        ProtocolResponse response = execute(line);

        if (echo_) {
            switch (response.status) {
                case ProtocolStatus::ok:
                    if (!response.data.empty()) {
                        std::cout << response.data << "\n";
                    } else {
                        std::cout << "OK\n";
                    }
                    break;
                case ProtocolStatus::value:
                    std::cout << response.data << "\n";
                    break;
                case ProtocolStatus::count:
                    std::cout << "(integer) " << response.data << "\n";
                    break;
                case ProtocolStatus::not_found:
                    std::cout << "(nil)\n";
                    break;
                case ProtocolStatus::error:
                    std::cout << "(error) " << response.data << "\n";
                    break;
                case ProtocolStatus::bye:
                    std::cout << "Bye\n";
                    break;
            }
        }

        if (response.status == ProtocolStatus::bye) {
            break;
        }
    }

    std::cout << "Disconnected\n";
}

void Client::setEcho(bool enabled) {
    echo_ = enabled;
}

bool Client::echo() const {
    return echo_;
}

bool Client::ensureConnected() {
    if (!connected_) {
        return connect();
    }
    return true;
}

} // namespace key_value_store
