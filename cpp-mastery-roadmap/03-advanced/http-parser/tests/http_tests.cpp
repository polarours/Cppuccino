#include "http.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_parse_get_request() {
    http::Request req;
    std::string raw = "GET /index.html HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "User-Agent: test\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == http::Method::GET, "expected GET method");
    expect(req.path == "/index.html", "expected /index.html path");
    expect(req.version == "HTTP/1.1", "expected HTTP/1.1");
    expect(req.getHeader("host") == "localhost", "expected Host header");
}

void test_parse_post_request() {
    http::Request req;
    std::string raw = "POST /api/data HTTP/1.1\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: 13\r\n"
                      "\r\n"
                      "{\"key\":\"val\"}";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == http::Method::POST, "expected POST method");
    expect(req.path == "/api/data", "expected /api/data path");
    expect(req.getHeader("content-type") == "application/json", "expected Content-Type");
    expect(req.body == "{\"key\":\"val\"}", "expected body");
}

void test_parse_multiple_headers() {
    http::Request req;
    std::string raw = "GET / HTTP/1.1\r\n"
                      "Host: example.com\r\n"
                      "Accept: text/html\r\n"
                      "Connection: keep-alive\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.headers.size() == 3, "expected 3 headers");
    expect(req.getHeader("accept") == "text/html", "expected Accept header");
}

void test_parse_invalid_request() {
    http::Request req;
    expect(!req.parse("INVALID"), "expected parse to fail");
    expect(!req.parse(""), "expected empty parse to fail");
}

void test_response_ok() {
    http::Response resp = http::Response::ok("Hello");
    std::string serialized = resp.serialize();
    expect(serialized.find("HTTP/1.1 200 OK") == 0, "expected 200 OK");
    expect(serialized.find("Hello") != std::string::npos, "expected body");
}

void test_response_not_found() {
    http::Response resp = http::Response::notFound();
    std::string serialized = resp.serialize();
    expect(serialized.find("404 Not Found") != std::string::npos, "expected 404");
}

void test_response_with_headers() {
    http::Response resp;
    resp.setStatus(201, "Created");
    resp.setHeader("X-Custom", "value");
    resp.setBody("created");

    std::string serialized = resp.serialize();
    expect(serialized.find("201 Created") != std::string::npos, "expected 201");
    // Project convention: header names are stored lowercased (as in
    // Request::parse / getHeader) because HTTP field names are
    // case-insensitive; setBody writes "content-length" too, and
    // normalizing avoids duplicate keys like Content-Length + content-length.
    expect(serialized.find("x-custom: value") != std::string::npos,
           "expected normalized custom header");
    expect(serialized.find("content-length: 7") != std::string::npos,
           "expected content length (lowercase)");
    // no duplicate differently-cased copies
    expect(serialized.find("Content-Length") == std::string::npos,
           "no mixed-case duplicate");
}

void test_method_names() {
    http::Request req;
    req.method = http::Method::GET;
    expect(req.methodName() == "GET", "expected GET");

    req.method = http::Method::POST;
    expect(req.methodName() == "POST", "expected POST");

    req.method = http::Method::DELETE;
    expect(req.methodName() == "DELETE", "expected DELETE");
}

void test_parse_put_request() {
    http::Request req;
    std::string raw = "PUT /resource/1 HTTP/1.1\r\n"
                      "Content-Type: application/json\r\n"
                      "\r\n"
                      "{\"name\":\"updated\"}";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == http::Method::PUT, "expected PUT method");
    expect(req.path == "/resource/1", "expected /resource/1 path");
}

void test_parse_head_request() {
    http::Request req;
    std::string raw = "HEAD / HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == http::Method::HEAD, "expected HEAD method");
}

void test_response_bad_request() {
    http::Response resp = http::Response::badRequest("Missing field");
    std::string serialized = resp.serialize();
    expect(serialized.find("400 Bad Request") != std::string::npos, "expected 400");
    expect(serialized.find("Missing field") != std::string::npos, "expected error message");
}

void test_response_internal_error() {
    http::Response resp = http::Response::internalError();
    std::string serialized = resp.serialize();
    expect(serialized.find("500 Internal Server Error") != std::string::npos, "expected 500");
}

// Regression: the body was read line by line and put back together, so
// it lost its last newline. A request that says how long its body is
// has to come back with exactly that many bytes, or a server that reads
// the body waits for a byte that never comes.
void test_body_keeps_its_last_newline() {
    http::Request req;
    req.parse("POST /x HTTP/1.1\r\nContent-Length: 6\r\n\r\nhello\n");

    expect(req.body.size() == 6, "expected 6 bytes, got " +
                                       std::to_string(req.body.size()));
    expect(req.body == "hello\n", "expected the newline to be there");
}

// Regression: a request that was parsed twice kept the headers and the
// body of the first one.
void test_parsing_twice_starts_from_scratch() {
    http::Request req;
    req.parse("GET /a HTTP/1.1\r\nX-First: 1\r\n\r\nfirst body");
    req.parse("GET /b HTTP/1.1\r\n\r\n");

    expect(req.getHeader("X-First").empty(), "expected the old header to be gone");
    expect(req.body.empty(), "expected the old body to be gone");
    expect(req.path == "/b", "expected the new path");
}

} // namespace

int main() {
    try {
        std::cout << "HTTP Parser Tests:\n";
        test_parse_get_request();
        std::cout << "  test_parse_get_request: PASS\n";

        test_parse_post_request();
        std::cout << "  test_parse_post_request: PASS\n";

        test_parse_multiple_headers();
        std::cout << "  test_parse_multiple_headers: PASS\n";

        test_parse_invalid_request();
        std::cout << "  test_parse_invalid_request: PASS\n";

        test_response_ok();
        std::cout << "  test_response_ok: PASS\n";

        test_response_not_found();
        std::cout << "  test_response_not_found: PASS\n";

        test_response_with_headers();
        std::cout << "  test_response_with_headers: PASS\n";

        test_method_names();
        std::cout << "  test_method_names: PASS\n";

        test_parse_put_request();
        std::cout << "  test_parse_put_request: PASS\n";

        test_parse_head_request();
        std::cout << "  test_parse_head_request: PASS\n";

        test_response_bad_request();
        std::cout << "  test_response_bad_request: PASS\n";

        test_response_internal_error();
        std::cout << "  test_response_internal_error: PASS\n";

        test_body_keeps_its_last_newline();
        std::cout << "  test_body_keeps_its_last_newline: PASS\n";

        test_parsing_twice_starts_from_scratch();
        std::cout << "  test_parsing_twice_starts_from_scratch: PASS\n";
    } catch (const std::exception& exception) {
        std::cerr << "http_parser_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nhttp_parser_tests passed" << std::endl;
    return 0;
}
