#include "server/server.hpp"

#include <exception>
#include <iostream>

int main() {
    tiny_http_server::Server server("0.0.0.0", 8082, 4);

    server.addRoute("GET", "/", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Welcome to tiny-http-server");
        return response;
    });

    server.addRoute("GET", "/hello", [](const tiny_http_server::HttpRequest&) {
        tiny_http_server::HttpResponse response;
        response.set_body("Hello from tiny-http-server");
        return response;
    });

    try {
        server.start();
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
