#include "web_server.hpp"

#include <iostream>
#include <string>

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

    std::cout << "Starting web server on port 8080...\n";
    std::cout << "Try: curl http://localhost:8080/\n";
    std::cout << "Try: curl http://localhost:8080/hello\n";
    std::cout << "Try: curl http://localhost:8080/time\n\n";

    std::cout << "Press Enter to stop server...\n";
    std::cin.get();

    server.stop();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
