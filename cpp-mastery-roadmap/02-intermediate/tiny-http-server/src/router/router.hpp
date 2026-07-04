#pragma once

#include "http/request.hpp"
#include "http/response.hpp"

#include <functional>
#include <map>
#include <string>

namespace tiny_http_server {

using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

class Router {
public:
    void addRoute(const std::string& method, const std::string& path, RouteHandler handler);
    HttpResponse route(const HttpRequest& request) const;

private:
    struct RouteKey {
        std::string method;
        std::string path;

        bool operator<(const RouteKey& other) const {
            if (method != other.method) return method < other.method;
            return path < other.path;
        }
    };

    std::map<RouteKey, RouteHandler> routes_;
};

} // namespace tiny_http_server
