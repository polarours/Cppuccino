#include "mini_http/app.hpp"

#include <iostream>

int main() {
    std::cout << "=== Mini HTTP Framework Demo ===\n\n";

    auto& app = mini_http::App::instance();

    // Middleware: logging
    app.use([](const mini_http::Request& req, mini_http::Response& res, std::function<void()> next) {
        std::cout << "[LOG] " << req.method() << " " << req.path() << "\n";
        next();
    });

    // Middleware: CORS
    app.use([](const mini_http::Request&, mini_http::Response& res, std::function<void()> next) {
        res.header("Access-Control-Allow-Origin", "*");
        next();
    });

    // Routes
    app.get("/", [](const mini_http::Request&) {
        return mini_http::Response::ok("Welcome to Mini HTTP Framework!")
            .contentType("text/plain");
    });

    app.get("/hello", [](const mini_http::Request&) {
        return mini_http::Response::ok("Hello, World!")
            .contentType("text/plain");
    });

    app.post("/echo", [](const mini_http::Request& req) {
        return mini_http::Response::ok(req.body())
            .contentType("application/json");
    });

    app.get("/status", [](const mini_http::Request&) {
        return mini_http::Response::ok()
            .contentType("application/json")
            .body("{\"status\":\"ok\",\"framework\":\"MiniHTTP\"}");
    });

    // Event listeners
    app.on("request.received", [](const mini_http::Event& e) {
        std::cout << "[EVENT] " << e.data << "\n";
    });

    // Test requests
    std::cout << "=== Simulated Requests ===\n\n";

    auto r1 = app.handleRaw("GET / HTTP/1.1\r\n\r\n");
    std::cout << "GET / -> " << r1.statusCode() << " " << r1.body() << "\n\n";

    auto r2 = app.handleRaw("GET /hello HTTP/1.1\r\n\r\n");
    std::cout << "GET /hello -> " << r2.statusCode() << " " << r2.body() << "\n\n";

    auto r3 = app.handleRaw("POST /echo HTTP/1.1\r\nContent-Type: text/plain\r\n\r\nHi there!");
    std::cout << "POST /echo -> " << r3.statusCode() << " " << r3.body() << "\n\n";

    auto r4 = app.handleRaw("GET /notfound HTTP/1.1\r\n\r\n");
    std::cout << "GET /notfound -> " << r4.statusCode() << "\n\n";

    std::cout << "=== Framework Info ===\n";
    std::cout << app.serve();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
