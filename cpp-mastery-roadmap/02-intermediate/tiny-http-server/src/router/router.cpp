#include "router.hpp"

namespace tiny_http_server {

std::string Router::route(const std::string& path) {

    if (path == "/") {
        return "Welcome to tiny-http-server";
    }

    if (path == "/hello") {
        return "Hello from router";
    }

    return "404 Not Found";
}

}