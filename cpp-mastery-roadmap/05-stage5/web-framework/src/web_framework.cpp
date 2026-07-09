#include "web_framework.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace web_framework {

// --- Request ---

std::string Request::getHeader(const std::string& name) const {
    auto it = headers.find(name);
    return (it != headers.end()) ? it->second : "";
}

std::string Request::getParam(const std::string& name) const {
    // Simple implementation - could be extended
    return "";
}

// --- Response ---

Response& Response::setStatus(int code, const std::string& message) {
    statusCode = code;
    statusMessage = message;
    return *this;
}

Response& Response::setHeader(const std::string& name, const std::string& value) {
    headers[name] = value;
    return *this;
}

Response& Response::setBody(const std::string& content) {
    body = content;
    headers["Content-Type"] = "text/plain";
    headers["Content-Length"] = std::to_string(body.size());
    return *this;
}

Response& Response::setJson(const std::string& json) {
    body = json;
    headers["Content-Type"] = "application/json";
    headers["Content-Length"] = std::to_string(body.size());
    return *this;
}

Response& Response::setHtml(const std::string& html) {
    body = html;
    headers["Content-Type"] = "text/html";
    headers["Content-Length"] = std::to_string(body.size());
    return *this;
}

std::string Response::serialize() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    for (const auto& [name, value] : headers) {
        oss << name << ": " << value << "\r\n";
    }
    oss << "\r\n" << body;
    return oss.str();
}

Response Response::ok(const std::string& body) {
    Response resp;
    resp.setStatus(200, "OK").setBody(body);
    return resp;
}

Response Response::json(const std::string& json) {
    Response resp;
    resp.setStatus(200, "OK").setJson(json);
    return resp;
}

Response Response::html(const std::string& html) {
    Response resp;
    resp.setStatus(200, "OK").setHtml(html);
    return resp;
}

Response Response::notFound() {
    Response resp;
    resp.setStatus(404, "Not Found").setBody("Not Found");
    return resp;
}

Response Response::badRequest(const std::string& error) {
    Response resp;
    resp.setStatus(400, "Bad Request").setBody(error.empty() ? "Bad Request" : error);
    return resp;
}

Response Response::internalError(const std::string& error) {
    Response resp;
    resp.setStatus(500, "Internal Server Error").setBody(error.empty() ? "Internal Server Error" : error);
    return resp;
}

// --- Router ---

void Router::get(const std::string& path, Handler handler) {
    routes_.push_back({"GET", path, std::move(handler)});
}

void Router::post(const std::string& path, Handler handler) {
    routes_.push_back({"POST", path, std::move(handler)});
}

void Router::put(const std::string& path, Handler handler) {
    routes_.push_back({"PUT", path, std::move(handler)});
}

void Router::del(const std::string& path, Handler handler) {
    routes_.push_back({"DELETE", path, std::move(handler)});
}

Response Router::route(const Request& request) {
    for (const auto& route : routes_) {
        if (route.method == request.method && route.path == request.path) {
            return route.handler(request);
        }
    }
    return Response::notFound();
}

// --- App ---

App::App() : port_(8080), server_fd_(-1), running_(false) {}
App::~App() { stop(); }

App& App::get(const std::string& path, Handler handler) {
    router_.get(path, std::move(handler));
    return *this;
}

App& App::post(const std::string& path, Handler handler) {
    router_.post(path, std::move(handler));
    return *this;
}

App& App::put(const std::string& path, Handler handler) {
    router_.put(path, std::move(handler));
    return *this;
}

App& App::del(const std::string& path, Handler handler) {
    router_.del(path, std::move(handler));
    return *this;
}

App& App::use(Middleware middleware) {
    middleware_.push_back(std::move(middleware));
    return *this;
}

Response App::handleRequest(const Request& request) {
    for (auto& middleware : middleware_) {
        Response response;
        if (!middleware(const_cast<Request&>(request), response)) {
            return response;
        }
    }
    return router_.route(request);
}

void App::listen(int port) {
    port_ = port;
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int reuse = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to bind");
    }

    if (::listen(server_fd_, 10) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to listen");
    }

    running_ = true;
    std::cout << "Server listening on port " << port << "\n";

    while (running_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) continue;

        char buffer[4096];
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            Request request;
            std::istringstream stream(buffer);
            std::string line;
            std::getline(stream, line);
            std::istringstream requestLine(line);
            requestLine >> request.method >> request.path >> request.version;

            Response response = handleRequest(request);
            std::string serialized = response.serialize();
            send(client_fd, serialized.c_str(), serialized.size(), 0);
        }
        close(client_fd);
    }
}

void App::stop() {
    running_ = false;
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

} // namespace web_framework
