#include "chat_server.hpp"

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    std::cout << "=== Chat Server Demo ===\n\n";

    // The port can be given on the command line. 8080 is the port
    // that everything on a machine wants, and three demos of this
    // repository used to fight over it.
    const int port = argc > 1 ? std::atoi(argv[1]) : 18081;

    chat_server::ChatServer server(port);

    server.setOnMessage([](const chat_server::Message& msg) {
        std::cout << "[LOG] " << msg.sender << ": " << msg.content << "\n";
    });

    server.setOnUserJoin([](const std::string& user) {
        std::cout << "[LOG] User joined: " << user << "\n";
    });

    server.setOnUserLeave([](const std::string& user) {
        std::cout << "[LOG] User left: " << user << "\n";
    });

    // The server runs in its own thread, so an exception it throws -- the
    // port being taken, for instance -- does not reach the main thread.
    // It is handed over instead.
    std::promise<std::string> startup;
    std::future<std::string> failure = startup.get_future();
    std::thread serverThread([&server, &startup]() {
        try {
            server.start();
            startup.set_value("");
        } catch (const std::exception& error) {
            startup.set_value(error.what());
        }
    });

    // If the server could not start, say so, instead of waiting for a
    // connection that will never come.
    if (failure.wait_for(std::chrono::milliseconds(500)) ==
        std::future_status::ready) {
        const std::string message = failure.get();
        if (!message.empty()) {
            std::cerr << "The chat server could not start: " << message << "\n";
            serverThread.join();
            return 1;
        }
    }

    std::cout << "Starting chat server on port " << port << "...\n";
    std::cout << "Connect with: telnet localhost " << port << "\n";
    std::cout << "Commands: /join <room>, /leave <room>, /rooms, /users, /quit\n\n";

    std::cout << "Press Enter to stop server...\n";
    std::cin.get();

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
