#pragma once

#include "request.hpp"
#include "response.hpp"
#include "handler.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace mini_http {

struct Route {
    std::string method;
    std::string path;
    Handler handler;
};

class Router {
public:
    void get(const std::string& path, Handler handler) {
        routes_.push_back({"GET", path, std::move(handler)});
    }

    void post(const std::string& path, Handler handler) {
        routes_.push_back({"POST", path, std::move(handler)});
    }

    void put(const std::string& path, Handler handler) {
        routes_.push_back({"PUT", path, std::move(handler)});
    }

    void del(const std::string& path, Handler handler) {
        routes_.push_back({"DELETE", path, std::move(handler)});
    }

    void addRoute(const std::string& method, const std::string& path, Handler handler) {
        routes_.push_back({method, path, std::move(handler)});
    }

    Response dispatch(const Request& req) const {
        for (const auto& route : routes_) {
            if (route.method == req.method() && route.path == req.path()) {
                return route.handler.handle(req);
            }
        }
        return Response::notFound();
    }

    std::size_t routeCount() const { return routes_.size(); }

    bool hasRoute(const std::string& method, const std::string& path) const {
        for (const auto& r : routes_) {
            if (r.method == method && r.path == path) return true;
        }
        return false;
    }

private:
    std::vector<Route> routes_;
};

} // namespace mini_http
