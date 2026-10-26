#include "web_server.hpp"

#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

int main() {
    std::cout << "=== Web Server Demo ===\n\n";

    web_server::WebServer server(8080);

    server.get("/", [](const web_server::Request&) {
        return web_server::Response::ok("Welcome to the web server!");
    });

    server.get("/hello", [](const web_server::Request&) {
        return web_server::Response::ok("Hello, World!");
    });

    server.get("/time", [](const web_server::Request&) {
        return web_server::Response::ok("Current time: now");
    });

    server.post("/echo", [](const web_server::Request& req) {
        return web_server::Response::ok(req.body);
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
    // request that will never come.
    if (failure.wait_for(std::chrono::milliseconds(500)) ==
        std::future_status::ready) {
        const std::string message = failure.get();
        if (!message.empty()) {
            std::cerr << "The web server could not start: " << message << "\n";
            serverThread.join();
            return 1;
        }
    }

    std::cout << "Starting web server on port 8080...\n";
    std::cout << "Try: curl http://localhost:8080/\n";
    std::cout << "Try: curl http://localhost:8080/hello\n";
    std::cout << "Try: curl http://localhost:8080/time\n\n";

    std::cout << "Press Enter to stop server...\n";
    std::cin.get();

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
