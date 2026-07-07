#include "http.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== HTTP Parser Demo ===\n\n";

    // Parse a GET request
    std::string rawRequest =
        "GET /api/users HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Accept: application/json\r\n"
        "User-Agent: Demo/1.0\r\n"
        "\r\n";

    http::Request request;
    if (request.parse(rawRequest)) {
        std::cout << "Parsed Request:\n";
        std::cout << "  Method: " << request.methodName() << "\n";
        std::cout << "  Path: " << request.path << "\n";
        std::cout << "  Version: " << request.version << "\n";
        std::cout << "  Headers:\n";
        for (const auto& [name, value] : request.headers) {
            std::cout << "    " << name << ": " << value << "\n";
        }
    }

    // Parse a POST request with body
    std::string rawPost =
        "POST /api/users HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{\"name\":\"Alice\",\"age\":25}";

    http::Request postRequest;
    if (postRequest.parse(rawPost)) {
        std::cout << "\nParsed POST Request:\n";
        std::cout << "  Method: " << postRequest.methodName() << "\n";
        std::cout << "  Body: " << postRequest.body << "\n";
    }

    // Create responses
    std::cout << "\nResponse Examples:\n";

    auto okResp = http::Response::ok("{\"status\":\"ok\"}");
    std::cout << "\n200 OK Response:\n" << okResp.serialize() << "\n";

    auto notFoundResp = http::Response::notFound();
    std::cout << "\n404 Not Found Response:\n" << notFoundResp.serialize() << "\n";

    auto errorResp = http::Response::badRequest("Invalid JSON");
    std::cout << "\n400 Bad Request Response:\n" << errorResp.serialize() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
