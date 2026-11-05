#include "web_framework.hpp"

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    std::cout << "=== Web Framework Demo ===\n\n";

    // The port can be given on the command line. 8080 is the port
    // that everything on a machine wants, and three demos of this
    // repository used to fight over it.
    const int port = argc > 1 ? std::atoi(argv[1]) : 18083;

    web_framework::App app;

    app.get("/", [](const web_framework::Request&) {
        return web_framework::Response::html("<h1>Welcome to Web Framework</h1>");
    });

    app.get("/api/users", [](const web_framework::Request&) {
        return web_framework::Response::json(R"({"users": ["Alice", "Bob"]})");
    });

    app.post("/api/users", []([[maybe_unused]] const web_framework::Request& req) {
        return web_framework::Response::json(R"({"status": "created"})").setStatus(201, "Created");
    });

    // The server runs in its own thread, so an exception it throws -- the
    // port being taken, for instance -- does not reach the main thread.
    // It is handed over instead.
    std::promise<std::string> startup;
    std::future<std::string> failure = startup.get_future();
    std::thread serverThread([&app, &startup, port]() {
        try {
            app.listen(port);
            startup.set_value("");
        } catch (const std::exception& error) {
            startup.set_value(error.what());
        }
    });

    // If the server could not start, say so, instead of waiting for a
    // request that will never come.
    if (failure.wait_for(std::chrono::milliseconds(500)) ==
        std::future_status::ready) {
        const std::string message = failure.get();
        if (!message.empty()) {
            std::cerr << "The web framework could not start: " << message << "\n";
            serverThread.join();
            return 1;
        }
    }

    std::cout << "Starting server on port " << port << "...\n";
    std::cout << "Try: curl http://localhost:" << port << "/\n";
    std::cout << "Try: curl http://localhost:" << port << "/api/users\n\n";

    std::cout << "Press Enter to stop server...\n";
    std::cin.get();

    app.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
