#include "router.hpp"

namespace tiny_http_server {

void Router::addRoute(const std::string& method, const std::string& path, RouteHandler handler) {
    routes_[{method, path}] = std::move(handler);
}

HttpResponse Router::route(const HttpRequest& request) const {
    RouteKey key{request.method(), request.path()};
    auto it = routes_.find(key);

    if (it != routes_.end()) {
        return it->second(request);
    }

    // Check if path exists but method is wrong
    for (const auto& [routeKey, _] : routes_) {
        if (routeKey.path == request.path()) {
            HttpResponse response;
            response.set_status(405, "Method Not Allowed");
            response.set_header("Allow", routeKey.method);
            response.set_body("Method Not Allowed");
            return response;
        }
    }

    HttpResponse response;
    response.set_status(404, "Not Found");
    response.set_body("404 Not Found");
    return response;
}

} // namespace tiny_http_server
