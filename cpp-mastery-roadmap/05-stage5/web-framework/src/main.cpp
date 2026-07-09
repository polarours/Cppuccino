#include "web_framework.hpp"

#include <iostream>

int main() {
    std::cout << "=== Web Framework Demo ===\n\n";

    web_framework::App app;

    app.get("/", [](const web_framework::Request&) {
        return web_framework::Response::html("<h1>Welcome to Web Framework</h1>");
    });

    app.get("/api/users", [](const web_framework::Request&) {
        return web_framework::Response::json(R"({"users": ["Alice", "Bob"]})");
    });

    app.post("/api/users", [](const web_framework::Request& req) {
        return web_framework::Response::json(R"({"status": "created"})").setStatus(201, "Created");
    });

    std::cout << "Starting server on port 8080...\n";
    std::cout << "Try: curl http://localhost:8080/\n";
    std::cout << "Try: curl http://localhost:8080/api/users\n\n";

    std::cout << "Press Enter to stop server...\n";
    std::cin.get();

    app.stop();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
