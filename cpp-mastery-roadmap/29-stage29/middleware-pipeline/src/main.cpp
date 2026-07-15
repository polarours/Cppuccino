#include "middleware_pipeline.hpp"

#include <iostream>

int main() {
    std::cout << "=== Middleware Pipeline Demo ===\n\n";

    middleware_pipeline::Pipeline pipeline;

    pipeline.use([](middleware_pipeline::Request& req, middleware_pipeline::Response& res, std::function<void()> next) {
        std::cout << "  Logger: " << req.method << " " << req.path << "\n";
        next();
    });

    pipeline.use([](middleware_pipeline::Request& req, middleware_pipeline::Response& res, std::function<void()> next) {
        res.headers["X-Powered-By"] = "Cppuccino";
        next();
    });

    middleware_pipeline::Request req{"/api/users", "GET", {}, ""};
    middleware_pipeline::Response res;
    pipeline.execute(req, res);

    std::cout << "  Status: " << res.status << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
