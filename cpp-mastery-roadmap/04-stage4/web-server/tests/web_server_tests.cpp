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

void test_request_post() {
    web_server::Request req;
    std::string raw = "POST /submit HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "Content-Type: application/json\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "POST", "expected POST method");
    expect(req.path == "/submit", "expected /submit path");
    expect(req.headers["Content-Type"] == "application/json", "expected Content-Type header");
}

void test_request_put() {
    web_server::Request req;
    std::string raw = "PUT /resource/1 HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "PUT", "expected PUT method");
    expect(req.path == "/resource/1", "expected /resource/1 path");
}

void test_request_delete() {
    web_server::Request req;
    std::string raw = "DELETE /resource/1 HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "DELETE", "expected DELETE method");
    expect(req.path == "/resource/1", "expected /resource/1 path");
}

void test_request_multiple_headers() {
    web_server::Request req;
    std::string raw = "GET / HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "Accept: text/html\r\n"
                      "User-Agent: test/1.0\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.headers.size() == 3, "expected 3 headers");
    expect(req.headers["Accept"] == "text/html", "expected Accept header");
    expect(req.headers["User-Agent"] == "test/1.0", "expected User-Agent header");
}

void test_response_bad_request() {
    web_server::Response resp = web_server::Response::badRequest();
    std::string serialized = resp.serialize();
    expect(serialized.find("400 Bad Request") != std::string::npos, "expected 400");
}

void test_response_internal_error() {
    web_server::Response resp = web_server::Response::internalError();
    std::string serialized = resp.serialize();
    expect(serialized.find("500 Internal Server Error") != std::string::npos, "expected 500");
}

void test_response_with_headers() {
    web_server::Response resp;
    resp.statusCode = 201;
    resp.statusMessage = "Created";
    resp.headers["X-Custom"] = "value";
    resp.headers["Content-Type"] = "application/json";
    resp.body = "{\"id\":1}";

    std::string serialized = resp.serialize();
    expect(serialized.find("201 Created") != std::string::npos, "expected 201");
    expect(serialized.find("X-Custom: value") != std::string::npos, "expected custom header");
    expect(serialized.find("Content-Type: application/json") != std::string::npos, "expected content type");
}

void test_request_invalid() {
    web_server::Request req;
    expect(!req.parse("INVALID"), "expected invalid request to fail");
    expect(!req.parse(""), "expected empty request to fail");
}

} // namespace

int main() {
    try {
        std::cout << "Web Server Tests:\n";
        test_request_parsing();
        std::cout << "  test_request_parsing: PASS\n";

        test_request_post();
        std::cout << "  test_request_post: PASS\n";

        test_request_put();
        std::cout << "  test_request_put: PASS\n";

        test_request_delete();
        std::cout << "  test_request_delete: PASS\n";

        test_request_multiple_headers();
        std::cout << "  test_request_multiple_headers: PASS\n";

        test_request_invalid();
        std::cout << "  test_request_invalid: PASS\n";

        test_response_ok();
        std::cout << "  test_response_ok: PASS\n";

        test_response_not_found();
        std::cout << "  test_response_not_found: PASS\n";

        test_response_bad_request();
        std::cout << "  test_response_bad_request: PASS\n";

        test_response_internal_error();
        std::cout << "  test_response_internal_error: PASS\n";

        test_response_with_headers();
        std::cout << "  test_response_with_headers: PASS\n";

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
