#include "http/request.hpp"
#include "http/response.hpp"
#include "router/router.hpp"
#include "thread-pool/thread_pool.hpp"

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_request_parsing_success() {
    tiny_http_server::HttpRequest request;
    const std::string raw_request =
        "GET /hello HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: tiny-test\r\n"
        "\r\n";

    expect(request.parse(raw_request), "expected valid request to parse");
    expect(request.method() == "GET", "expected method to be GET");
    expect(request.path() == "/hello", "expected path to be /hello");
    expect(request.version() == "HTTP/1.1", "expected HTTP version");
    expect(request.header("Host") == "localhost", "expected Host header");
    expect(request.header("User-Agent") == "tiny-test", "expected User-Agent header");
}

void test_request_parsing_failure() {
    tiny_http_server::HttpRequest request;
    expect(!request.parse("BROKEN REQUEST\r\n\r\n"), "expected invalid request to fail");
}

void test_request_no_headers() {
    tiny_http_server::HttpRequest request;
    const std::string raw_request =
        "GET / HTTP/1.0\r\n"
        "\r\n";

    expect(request.parse(raw_request), "expected request without custom headers to parse");
    expect(request.method() == "GET", "expected method GET");
    expect(request.path() == "/", "expected path /");
    expect(request.version() == "HTTP/1.0", "expected HTTP/1.0");
    expect(request.header("Nonexistent").empty(), "expected empty header for missing key");
}

void test_request_multiple_headers() {
    tiny_http_server::HttpRequest request;
    const std::string raw_request =
        "POST /data HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Type: application/json\r\n"
        "Accept: text/html\r\n"
        "X-Custom-Header: custom-value\r\n"
        "\r\n";

    expect(request.parse(raw_request), "expected request with multiple headers");
    expect(request.method() == "POST", "expected POST method");
    expect(request.path() == "/data", "expected /data path");
    expect(request.header("Content-Type") == "application/json", "expected Content-Type");
    expect(request.header("Accept") == "text/html", "expected Accept");
    expect(request.header("X-Custom-Header") == "custom-value", "expected custom header");
}

void test_request_empty_path() {
    tiny_http_server::HttpRequest request;
    const std::string raw_request =
        "GET  HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    expect(!request.parse(raw_request), "expected request with empty path to fail");
}

void test_request_different_methods() {
    tiny_http_server::HttpRequest head_request;
    expect(head_request.parse("HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected HEAD to parse");
    expect(head_request.method() == "HEAD", "expected HEAD method");

    tiny_http_server::HttpRequest put_request;
    expect(put_request.parse("PUT /resource HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected PUT to parse");
    expect(put_request.method() == "PUT", "expected PUT method");

    tiny_http_server::HttpRequest delete_request;
    expect(delete_request.parse("DELETE /resource HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected DELETE to parse");
    expect(delete_request.method() == "DELETE", "expected DELETE method");
}

void test_response_serialization() {
    tiny_http_server::HttpResponse response;
    response.set_status(201, "Created");
    response.set_header("X-Test", "true");
    response.set_body("payload");

    const std::string serialized = response.to_string();
    expect(
        serialized.find("HTTP/1.1 201 Created\r\n") == 0,
        "expected serialized status line"
    );
    expect(
        serialized.find("Content-Length: 7\r\n") != std::string::npos,
        "expected content length header"
    );
    expect(
        serialized.find("X-Test: true\r\n") != std::string::npos,
        "expected custom header"
    );
    expect(
        serialized.size() >= 7 && serialized.substr(serialized.size() - 7) == "payload",
        "expected serialized body"
    );
}

void test_response_multiple_headers() {
    tiny_http_server::HttpResponse response;
    response.set_status(200, "OK");
    response.set_header("Content-Type", "text/plain");
    response.set_header("Cache-Control", "no-cache");
    response.set_header("X-Request-Id", "12345");
    response.set_body("test");

    const std::string serialized = response.to_string();
    expect(serialized.find("Content-Type: text/plain\r\n") != std::string::npos, "expected Content-Type");
    expect(serialized.find("Cache-Control: no-cache\r\n") != std::string::npos, "expected Cache-Control");
    expect(serialized.find("X-Request-Id: 12345\r\n") != std::string::npos, "expected X-Request-Id");
}

void test_response_empty_body() {
    tiny_http_server::HttpResponse response;
    response.set_status(204, "No Content");
    response.set_body("");

    const std::string serialized = response.to_string();
    expect(serialized.find("HTTP/1.1 204 No Content\r\n") == 0, "expected 204 status");
    expect(serialized.find("Content-Length: 0\r\n") != std::string::npos, "expected Content-Length 0");
}

void test_response_special_chars_body() {
    tiny_http_server::HttpResponse response;
    response.set_status(200, "OK");
    response.set_body("Hello <World> & \"Friends\"");

    const std::string serialized = response.to_string();
    expect(
        serialized.find("Hello <World> & \"Friends\"") != std::string::npos,
        "expected body with special characters"
    );
}

void test_router_routes() {
    tiny_http_server::Router router;

    router.addRoute("GET", "/", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Welcome to tiny-http-server");
        return response;
    });

    router.addRoute("GET", "/hello", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Hello from router");
        return response;
    });

    tiny_http_server::HttpRequest root_request;
    expect(root_request.parse("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected root request");
    const std::string root_response = router.route(root_request).to_string();
    expect(root_response.find("HTTP/1.1 200 OK\r\n") == 0, "expected 200 for root route");
    expect(
        root_response.find("Welcome to tiny-http-server") != std::string::npos,
        "expected root route body"
    );

    tiny_http_server::HttpRequest missing_request;
    expect(
        missing_request.parse("GET /missing HTTP/1.1\r\nHost: localhost\r\n\r\n"),
        "expected missing request"
    );
    const std::string missing_response = router.route(missing_request).to_string();
    expect(
        missing_response.find("HTTP/1.1 404 Not Found\r\n") == 0,
        "expected 404 for missing route"
    );

    tiny_http_server::HttpRequest method_request;
    expect(
        method_request.parse("POST /hello HTTP/1.1\r\nHost: localhost\r\n\r\n"),
        "expected POST request"
    );
    const std::string method_response = router.route(method_request).to_string();
    expect(
        method_response.find("HTTP/1.1 405 Method Not Allowed\r\n") == 0,
        "expected 405 for unsupported method"
    );
    expect(
        method_response.find("Allow: GET\r\n") != std::string::npos,
        "expected Allow header for unsupported method"
    );
}

void test_router_put_delete_patch() {
    tiny_http_server::Router router;

    router.addRoute("GET", "/hello", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Hello from router");
        return response;
    });

    tiny_http_server::HttpRequest put_request;
    expect(put_request.parse("PUT /hello HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected PUT request");
    const std::string put_response = router.route(put_request).to_string();
    expect(put_response.find("405 Method Not Allowed") != std::string::npos, "expected 405 for PUT");

    tiny_http_server::HttpRequest delete_request;
    expect(delete_request.parse("DELETE /hello HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected DELETE request");
    const std::string delete_response = router.route(delete_request).to_string();
    expect(delete_response.find("405 Method Not Allowed") != std::string::npos, "expected 405 for DELETE");

    tiny_http_server::HttpRequest patch_request;
    expect(patch_request.parse("PATCH /hello HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected PATCH request");
    const std::string patch_response = router.route(patch_request).to_string();
    expect(patch_response.find("405 Method Not Allowed") != std::string::npos, "expected 405 for PATCH");
}

void test_router_hello_route() {
    tiny_http_server::Router router;

    router.addRoute("GET", "/hello", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Hello from router");
        return response;
    });

    tiny_http_server::HttpRequest request;
    expect(request.parse("GET /hello HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected /hello request");
    const std::string response = router.route(request).to_string();
    expect(response.find("HTTP/1.1 200 OK\r\n") == 0, "expected 200 for /hello");
    expect(response.find("Hello from router") != std::string::npos, "expected hello body");
}

void test_thread_pool_executes_tasks() {
    tiny_http_server::ThreadPool pool(2);
    std::mutex mutex;
    std::condition_variable condition;
    int completed = 0;

    for (int i = 0; i < 4; ++i) {
        pool.enqueue([&]() {
            std::lock_guard<std::mutex> lock(mutex);
            ++completed;
            condition.notify_one();
        });
    }

    std::unique_lock<std::mutex> lock(mutex);
    const bool all_tasks_finished = condition.wait_for(
        lock,
        std::chrono::seconds(2),
        [&]() { return completed == 4; }
    );

    expect(all_tasks_finished, "expected thread pool tasks to finish");
}

void test_thread_pool_many_tasks() {
    tiny_http_server::ThreadPool pool(3);
    std::mutex mutex;
    std::condition_variable condition;
    int completed = 0;
    const int total_tasks = 20;

    for (int i = 0; i < total_tasks; ++i) {
        pool.enqueue([&]() {
            std::lock_guard<std::mutex> lock(mutex);
            ++completed;
            condition.notify_one();
        });
    }

    std::unique_lock<std::mutex> lock(mutex);
    const bool all_tasks_finished = condition.wait_for(
        lock,
        std::chrono::seconds(5),
        [&]() { return completed == total_tasks; }
    );

    expect(all_tasks_finished, "expected all 20 tasks to finish");
}

void test_router_handler_registration() {
    tiny_http_server::Router router;

    router.addRoute("GET", "/custom", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Custom handler");
        return response;
    });

    tiny_http_server::HttpRequest request;
    expect(request.parse("GET /custom HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected /custom request");
    const std::string response = router.route(request).to_string();
    expect(response.find("Custom handler") != std::string::npos, "expected custom handler response");
}

void test_router_multiple_methods() {
    tiny_http_server::Router router;

    router.addRoute("GET", "/resource", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("GET response");
        return response;
    });

    router.addRoute("POST", "/resource", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_status(201, "Created");
        response.set_body("POST response");
        return response;
    });

    tiny_http_server::HttpRequest get_request;
    expect(get_request.parse("GET /resource HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected GET request");
    const std::string get_response = router.route(get_request).to_string();
    expect(get_response.find("GET response") != std::string::npos, "expected GET response");

    tiny_http_server::HttpRequest post_request;
    expect(post_request.parse("POST /resource HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected POST request");
    const std::string post_response = router.route(post_request).to_string();
    expect(post_response.find("POST response") != std::string::npos, "expected POST response");
    expect(post_response.find("201 Created") != std::string::npos, "expected 201 status");
}

void test_router_handler_with_request_data() {
    tiny_http_server::Router router;

    router.addRoute("GET", "/greet", [](const tiny_http_server::HttpRequest& req) {
        tiny_http_server::HttpResponse response;
        response.set_body("Hello from " + req.path());
        return response;
    });

    tiny_http_server::HttpRequest request;
    expect(request.parse("GET /greet HTTP/1.1\r\nHost: localhost\r\n\r\n"), "expected /greet request");
    const std::string response = router.route(request).to_string();
    expect(response.find("Hello from /greet") != std::string::npos, "expected path in response");
}

} // namespace

int main() {
    try {
        test_request_parsing_success();
        test_request_parsing_failure();
        test_request_no_headers();
        test_request_multiple_headers();
        test_request_empty_path();
        test_request_different_methods();
        test_response_serialization();
        test_response_multiple_headers();
        test_response_empty_body();
        test_response_special_chars_body();
        test_router_routes();
        test_router_put_delete_patch();
        test_router_hello_route();
        test_thread_pool_executes_tasks();
        test_thread_pool_many_tasks();
        test_router_handler_registration();
        test_router_multiple_methods();
        test_router_handler_with_request_data();
    } catch (const std::exception& exception) {
        std::cerr << "tiny_http_server_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "tiny_http_server_tests passed" << std::endl;
    return 0;
}
