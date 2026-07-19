#include "mini_http/app.hpp"
#include "mini_http/request.hpp"
#include "mini_http/response.hpp"
#include "mini_http/handler.hpp"
#include "mini_http/router.hpp"
#include "mini_http/middleware.hpp"
#include "mini_http/event_bus.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// --- Request Tests ---
void test_request_parse() {
    std::string raw = "GET /hello HTTP/1.1\r\nHost: localhost\r\nAccept: text/html\r\n\r\n";
    auto req = mini_http::Request::parse(raw);

    expect(req.method() == "GET", "expected GET");
    expect(req.path() == "/hello", "expected /hello");
    expect(req.header("Host") == "localhost", "expected localhost");
    expect(req.header("Accept") == "text/html", "expected text/html");
}

void test_request_post_body() {
    std::string raw = "POST /data HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{\"key\":\"value\"}";
    auto req = mini_http::Request::parse(raw);

    expect(req.method() == "POST", "expected POST");
    expect(req.path() == "/data", "expected /data");
    expect(req.body() == "{\"key\":\"value\"}", "expected body");
}

// --- Response Builder Tests ---
void test_response_ok() {
    auto res = mini_http::Response::ok("Hello");
    expect(res.statusCode() == 200, "expected 200");
    expect(res.body() == "Hello", "expected Hello");
}

void test_response_not_found() {
    auto res = mini_http::Response::notFound();
    expect(res.statusCode() == 404, "expected 404");
}

void test_response_builder() {
    auto res = mini_http::Response::ok()
        .header("X-Custom", "test")
        .contentType("application/json")
        .body("{\"ok\":true}");

    expect(res.header("X-Custom") == "test", "expected test");
    expect(res.header("Content-Type") == "application/json", "expected json");
    expect(res.body() == "{\"ok\":true}", "expected body");
}

void test_response_to_string() {
    auto res = mini_http::Response::ok("Hi");
    auto str = res.toString();
    expect(str.find("HTTP/1.1 200 OK") != std::string::npos, "expected status line");
    expect(str.find("Hi") != std::string::npos, "expected body");
}

// --- Router Tests ---
void test_router_basic() {
    mini_http::Router router;
    router.get("/hello", [](const mini_http::Request&) {
        return mini_http::Response::ok("Hello, World!");
    });

    mini_http::Request req("GET", "/hello");
    auto res = router.dispatch(req);
    expect(res.body() == "Hello, World!", "expected Hello, World!");
}

void test_router_not_found() {
    mini_http::Router router;
    router.get("/exists", [](const mini_http::Request&) {
        return mini_http::Response::ok("ok");
    });

    mini_http::Request req("GET", "/missing");
    auto res = router.dispatch(req);
    expect(res.statusCode() == 404, "expected 404");
}

void test_router_multiple_routes() {
    mini_http::Router router;
    router.get("/a", [](const mini_http::Request&) { return mini_http::Response::ok("A"); });
    router.post("/b", [](const mini_http::Request&) { return mini_http::Response::ok("B"); });
    router.get("/c", [](const mini_http::Request&) { return mini_http::Response::ok("C"); });

    expect(router.routeCount() == 3, "expected 3 routes");
    expect(router.hasRoute("GET", "/a"), "expected GET /a");
    expect(router.hasRoute("POST", "/b"), "expected POST /b");
    expect(!router.hasRoute("DELETE", "/a"), "expected no DELETE /a");
}

// --- Middleware Tests ---
void test_middleware_pipeline() {
    mini_http::MiddlewarePipeline pipeline;
    std::string order;

    pipeline.use([&order](const mini_http::Request&, mini_http::Response& res, std::function<void()> next) {
        order += "A";
        next();
        order += "A'";
    });
    pipeline.use([&order](const mini_http::Request&, mini_http::Response&, std::function<void()> next) {
        order += "B";
        next();
        order += "B'";
    });

    mini_http::Request req("GET", "/");
    pipeline.execute(req);

    expect(order == "ABB'A'", "expected ABB'A'");
}

void test_middleware_short_circuit() {
    mini_http::MiddlewarePipeline pipeline;
    bool reached = false;

    pipeline.use([](const mini_http::Request&, mini_http::Response& res, std::function<void()>) {
        res = mini_http::Response::ok("blocked");
    });
    pipeline.use([&reached](const mini_http::Request&, mini_http::Response&, std::function<void()> next) {
        reached = true;
        next();
    });

    mini_http::Request req("GET", "/");
    auto res = pipeline.execute(req);

    expect(res.body() == "blocked", "expected blocked");
    expect(!reached, "expected short-circuit");
}

// --- EventBus Tests ---
void test_event_bus() {
    mini_http::EventBus bus;

    int count = 0;
    bus.subscribe("test", [&count](const mini_http::Event&) { count++; });
    bus.subscribe("test", [&count](const mini_http::Event&) { count++; });

    bus.publish({"test", "data"});
    expect(count == 2, "expected 2 handlers");
    expect(bus.handlerCount("test") == 2, "expected 2");
}

// --- Handler Type Erasure Tests ---
void test_handler_type_erasure() {
    mini_http::Handler h([](const mini_http::Request&) {
        return mini_http::Response::ok("typed");
    });

    mini_http::Request req("GET", "/");
    auto res = h.handle(req);
    expect(res.body() == "typed", "expected typed");
}

// --- App Facade Tests ---
void test_app_facade() {
    auto& app = mini_http::App::instance();

    app.router().get("/", [](const mini_http::Request&) {
        return mini_http::Response::ok("Home");
    });

    mini_http::Request req("GET", "/");
    auto res = app.handle(req);
    expect(res.body() == "Home", "expected Home");
}

void test_app_handle_raw() {
    auto& app = mini_http::App::instance();

    app.router().get("/api", [](const mini_http::Request&) {
        return mini_http::Response::ok("API");
    });

    std::string raw = "GET /api HTTP/1.1\r\nHost: test\r\n\r\n";
    auto res = app.handleRaw(raw);
    expect(res.body() == "API", "expected API");
}

void test_app_events() {
    auto& app = mini_http::App::instance();
    app.events().clear();

    int requestCount = 0;
    app.on("request.received", [&requestCount](const mini_http::Event&) { requestCount++; });

    app.router().get("/count", [](const mini_http::Request&) {
        return mini_http::Response::ok("ok");
    });

    mini_http::Request req("GET", "/count");
    app.handle(req);

    expect(requestCount >= 1, "expected at least 1 request event");
}

void test_app_serve() {
    auto& app = mini_http::App::instance();
    auto info = app.serve();
    expect(info.find("MiniHTTP") != std::string::npos, "expected framework name");
}

} // namespace

int main() {
    try {
        std::cout << "=== Mini HTTP Framework Tests ===\n\n";

        std::cout << "Request:\n";
        test_request_parse();
        std::cout << "  test_request_parse: PASS\n";
        test_request_post_body();
        std::cout << "  test_request_post_body: PASS\n";

        std::cout << "\nResponse Builder:\n";
        test_response_ok();
        std::cout << "  test_response_ok: PASS\n";
        test_response_not_found();
        std::cout << "  test_response_not_found: PASS\n";
        test_response_builder();
        std::cout << "  test_response_builder: PASS\n";
        test_response_to_string();
        std::cout << "  test_response_to_string: PASS\n";

        std::cout << "\nRouter (Strategy):\n";
        test_router_basic();
        std::cout << "  test_router_basic: PASS\n";
        test_router_not_found();
        std::cout << "  test_router_not_found: PASS\n";
        test_router_multiple_routes();
        std::cout << "  test_router_multiple_routes: PASS\n";

        std::cout << "\nMiddleware Pipeline:\n";
        test_middleware_pipeline();
        std::cout << "  test_middleware_pipeline: PASS\n";
        test_middleware_short_circuit();
        std::cout << "  test_middleware_short_circuit: PASS\n";

        std::cout << "\nEvent Bus (Observer):\n";
        test_event_bus();
        std::cout << "  test_event_bus: PASS\n";

        std::cout << "\nHandler (Type Erasure):\n";
        test_handler_type_erasure();
        std::cout << "  test_handler_type_erasure: PASS\n";

        std::cout << "\nApp (Facade + Singleton):\n";
        test_app_facade();
        std::cout << "  test_app_facade: PASS\n";
        test_app_handle_raw();
        std::cout << "  test_app_handle_raw: PASS\n";
        test_app_events();
        std::cout << "  test_app_events: PASS\n";
        test_app_serve();
        std::cout << "  test_app_serve: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "\nTests FAILED: " << exception << std::endl;
        return 1;
    }

    std::cout << "\n=== All 19 tests passed ===" << std::endl;
    return 0;
}
