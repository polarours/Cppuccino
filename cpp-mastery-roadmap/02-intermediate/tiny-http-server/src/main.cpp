#include "server/server.hpp"

#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <string>
#include <thread>

int main() {
    constexpr int kPort = 8082;

    tiny_http_server::Server server("0.0.0.0", kPort, 4);

    server.addRoute("GET", "/", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Welcome to tiny-http-server");
        return response;
    });

    server.addRoute("GET", "/hello", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Hello from tiny-http-server");
        return response;
    });

    // The server runs on its own thread, so that the demo can say what it
    // is doing and wait for the reader to stop it. Without that, the demo
    // printed nothing at all and looked like a program that had hung.
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
            std::cerr << "The tiny http server could not start: " << message
                      << "\n";
            serverThread.join();
            return 1;
        }
    }

    std::cout << "Starting tiny http server on port " << kPort << "...\n";
    std::cout << "Try: curl http://127.0.0.1:" << kPort << "/\n";
    std::cout << "Try: curl http://127.0.0.1:" << kPort << "/hello\n\n";

    std::cout << "Press Enter to stop server...\n";
    std::cin.get();

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
