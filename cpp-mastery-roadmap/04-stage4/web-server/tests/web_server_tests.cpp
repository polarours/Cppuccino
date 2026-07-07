#include "web_server.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_request_parsing() {
    web_server::Request req;
    std::string raw = "GET /hello HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "GET", "expected GET method");
    expect(req.path == "/hello", "expected /hello path");
    expect(req.headers["Host"] == "localhost", "expected Host header");
}

void test_response_ok() {
    web_server::Response resp = web_server::Response::ok("Hello");
    std::string serialized = resp.serialize();
    expect(serialized.find("200 OK") != std::string::npos, "expected 200 OK");
    expect(serialized.find("Hello") != std::string::npos, "expected body");
}

void test_response_not_found() {
    web_server::Response resp = web_server::Response::notFound();
    std::string serialized = resp.serialize();
    expect(serialized.find("404 Not Found") != std::string::npos, "expected 404");
}

void test_server_creation() {
    web_server::WebServer server(18001);
    expect(!server.isRunning(), "expected not running initially");
}

void test_route_registration() {
    web_server::WebServer server(18002);
    server.get("/", [](const web_server::Request&) {
        return web_server::Response::ok("Home");
    });
    server.get("/about", [](const web_server::Request&) {
        return web_server::Response::ok("About");
    });
    // Just test that routes can be registered without crash
    expect(true, "expected route registration to succeed");
}

} // namespace

int main() {
    try {
        std::cout << "Web Server Tests:\n";
        test_request_parsing();
        std::cout << "  test_request_parsing: PASS\n";

        test_response_ok();
        std::cout << "  test_response_ok: PASS\n";

        test_response_not_found();
        std::cout << "  test_response_not_found: PASS\n";

        test_server_creation();
        std::cout << "  test_server_creation: PASS\n";

        test_route_registration();
        std::cout << "  test_route_registration: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "web_server_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nweb_server_tests passed" << std::endl;
    return 0;
}
