#include "chat_server.hpp"

#include <iostream>
#include <string>
#include <thread>

int main() {
    std::cout << "=== Chat Server Demo ===\n\n";

    chat_server::ChatServer server(8080);

    server.setOnMessage([](const chat_server::Message& msg) {
        std::cout << "[LOG] " << msg.sender << ": " << msg.content << "\n";
    });

    server.setOnUserJoin([](const std::string& user) {
        std::cout << "[LOG] User joined: " << user << "\n";
    });

    server.setOnUserLeave([](const std::string& user) {
        std::cout << "[LOG] User left: " << user << "\n";
    });

    std::cout << "Starting chat server on port 8080...\n";
    std::cout << "Connect with: telnet localhost 8080\n";
    std::cout << "Commands: /join <room>, /leave <room>, /rooms, /users, /quit\n\n";

    std::thread serverThread([&server]() {
        server.start();
    });

    std::cout << "Press Enter to stop server...\n";
    std::cin.get();

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
