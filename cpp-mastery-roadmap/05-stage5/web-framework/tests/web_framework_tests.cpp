#include "web_framework.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_response_ok() {
    web_framework::Response resp = web_framework::Response::ok("Hello");
    std::string serialized = resp.serialize();
    expect(serialized.find("200 OK") != std::string::npos, "expected 200 OK");
    expect(serialized.find("Hello") != std::string::npos, "expected body");
}

void test_response_json() {
    web_framework::Response resp = web_framework::Response::json("{\"key\":\"value\"}");
    std::string serialized = resp.serialize();
    expect(serialized.find("application/json") != std::string::npos, "expected JSON content type");
    expect(serialized.find("{\"key\":\"value\"}") != std::string::npos, "expected JSON body");
}

void test_response_html() {
    web_framework::Response resp = web_framework::Response::html("<h1>Hello</h1>");
    std::string serialized = resp.serialize();
    expect(serialized.find("text/html") != std::string::npos, "expected HTML content type");
    expect(serialized.find("<h1>Hello</h1>") != std::string::npos, "expected HTML body");
}

void test_response_not_found() {
    web_framework::Response resp = web_framework::Response::notFound();
    std::string serialized = resp.serialize();
    expect(serialized.find("404 Not Found") != std::string::npos, "expected 404");
}

void test_response_bad_request() {
    web_framework::Response resp = web_framework::Response::badRequest("Invalid input");
    std::string serialized = resp.serialize();
    expect(serialized.find("400 Bad Request") != std::string::npos, "expected 400");
    expect(serialized.find("Invalid input") != std::string::npos, "expected error message");
}

void test_response_internal_error() {
    web_framework::Response resp = web_framework::Response::internalError();
    std::string serialized = resp.serialize();
    expect(serialized.find("500 Internal Server Error") != std::string::npos, "expected 500");
}

void test_response_chaining() {
    web_framework::Response resp;
    resp.setStatus(201, "Created")
       .setHeader("X-Custom", "value")
       .setBody("created");

    std::string serialized = resp.serialize();
    expect(serialized.find("201 Created") != std::string::npos, "expected 201");
    expect(serialized.find("X-Custom: value") != std::string::npos, "expected custom header");
}

void test_router() {
    web_framework::Router router;

    router.get("/", [](const web_framework::Request&) {
        return web_framework::Response::ok("Home");
    });

    router.get("/about", [](const web_framework::Request&) {
        return web_framework::Response::ok("About");
    });

    web_framework::Request req;
    req.method = "GET";
    req.path = "/";

    auto resp = router.route(req);
    expect(resp.statusCode == 200, "expected 200");
    expect(resp.body == "Home", "expected Home");
}

void test_router_not_found() {
    web_framework::Router router;

    router.get("/", [](const web_framework::Request&) {
        return web_framework::Response::ok("Home");
    });

    web_framework::Request req;
    req.method = "GET";
    req.path = "/missing";

    auto resp = router.route(req);
    expect(resp.statusCode == 404, "expected 404");
}

void test_app_routing() {
    web_framework::App app;

    app.get("/", [](const web_framework::Request&) {
        return web_framework::Response::ok("Home");
    });

    app.get("/hello", [](const web_framework::Request&) {
        return web_framework::Response::ok("Hello");
    });

    web_framework::Request req;
    req.method = "GET";
    req.path = "/hello";

    auto resp = app.handleRequest(req);
    expect(resp.statusCode == 200, "expected 200");
    expect(resp.body == "Hello", "expected Hello");
}

} // namespace

int main() {
    try {
        std::cout << "Web Framework Tests:\n";
        test_response_ok();
        std::cout << "  test_response_ok: PASS\n";

        test_response_json();
        std::cout << "  test_response_json: PASS\n";

        test_response_html();
        std::cout << "  test_response_html: PASS\n";

        test_response_not_found();
        std::cout << "  test_response_not_found: PASS\n";

        test_response_bad_request();
        std::cout << "  test_response_bad_request: PASS\n";

        test_response_internal_error();
        std::cout << "  test_response_internal_error: PASS\n";

        test_response_chaining();
        std::cout << "  test_response_chaining: PASS\n";

        test_router();
        std::cout << "  test_router: PASS\n";

        test_router_not_found();
        std::cout << "  test_router_not_found: PASS\n";

        test_app_routing();
        std::cout << "  test_app_routing: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "web_framework_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nweb_framework_tests passed" << std::endl;
    return 0;
}
