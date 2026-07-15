#include "middleware_pipeline.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_single_middleware() {
    middleware_pipeline::Pipeline pipeline;
    pipeline.use([](middleware_pipeline::Request&, middleware_pipeline::Response& res, std::function<void()> next) {
        res.body = "processed";
        next();
    });

    middleware_pipeline::Request req;
    middleware_pipeline::Response res;
    pipeline.execute(req, res);

    expect(res.body == "processed", "expected processed");
}

void test_chain_order() {
    middleware_pipeline::Pipeline pipeline;
    std::string order;

    pipeline.use([&order](middleware_pipeline::Request&, middleware_pipeline::Response&, std::function<void()> next) {
        order += "A";
        next();
        order += "A'";
    });
    pipeline.use([&order](middleware_pipeline::Request&, middleware_pipeline::Response&, std::function<void()> next) {
        order += "B";
        next();
        order += "B'";
    });

    middleware_pipeline::Request req;
    middleware_pipeline::Response res;
    pipeline.execute(req, res);

    expect(order == "ABB'A'", "expected ABB'A'");
}

void test_short_circuit() {
    middleware_pipeline::Pipeline pipeline;
    bool reached = false;

    pipeline.use([](middleware_pipeline::Request&, middleware_pipeline::Response& res, std::function<void()>) {
        res.status = 401;
    });
    pipeline.use([&reached](middleware_pipeline::Request&, middleware_pipeline::Response&, std::function<void()> next) {
        reached = true;
        next();
    });

    middleware_pipeline::Request req;
    middleware_pipeline::Response res;
    pipeline.execute(req, res);

    expect(res.status == 401, "expected 401");
    expect(!reached, "expected short-circuit");
}

} // namespace

int main() {
    try {
        std::cout << "Middleware Pipeline Tests:\n";
        test_single_middleware();
        std::cout << "  test_single_middleware: PASS\n";

        test_chain_order();
        std::cout << "  test_chain_order: PASS\n";

        test_short_circuit();
        std::cout << "  test_short_circuit: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "middleware_pipeline_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nmiddleware_pipeline_tests passed" << std::endl;
    return 0;
}
