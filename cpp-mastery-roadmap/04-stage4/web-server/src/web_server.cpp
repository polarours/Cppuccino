#include "web_server.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace web_server {

bool Request::parse(const std::string& raw) {
    std::istringstream stream(raw);
    std::string line;

    if (!std::getline(stream, line)) return false;

    std::istringstream requestLine(line);
    if (!(requestLine >> method >> path >> version)) return false;

    while (std::getline(stream, line)) {
        if (line == "\r" || line.empty()) break;

        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            if (!value.empty() && value[0] == ' ') value = value.substr(1);
            if (!value.empty() && value.back() == '\r') value.pop_back();
            headers[name] = value;
        }
    }

    return true;
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
    resp.statusCode = 200;
    resp.statusMessage = "OK";
    resp.headers["Content-Type"] = "text/plain";
    resp.headers["Content-Length"] = std::to_string(body.size());
    resp.body = body;
    return resp;
}

Response Response::notFound() {
    Response resp;
    resp.statusCode = 404;
    resp.statusMessage = "Not Found";
    resp.headers["Content-Type"] = "text/plain";
    resp.body = "Not Found";
    resp.headers["Content-Length"] = std::to_string(resp.body.size());
    return resp;
}

Response Response::badRequest() {
    Response resp;
    resp.statusCode = 400;
    resp.statusMessage = "Bad Request";
    resp.headers["Content-Type"] = "text/plain";
    resp.body = "Bad Request";
    resp.headers["Content-Length"] = std::to_string(resp.body.size());
    return resp;
}

Response Response::internalError() {
    Response resp;
    resp.statusCode = 500;
    resp.statusMessage = "Internal Server Error";
    resp.headers["Content-Type"] = "text/plain";
    resp.body = "Internal Server Error";
    resp.headers["Content-Length"] = std::to_string(resp.body.size());
    return resp;
}

WebServer::WebServer(int port)
    : port_(port)
    , server_fd_(-1)
    , running_(false) {}

WebServer::~WebServer() {
    stop();
}

void WebServer::get(const std::string& path, Handler handler) {
    routes_.push_back({"GET", path, std::move(handler)});
}

void WebServer::post(const std::string& path, Handler handler) {
    routes_.push_back({"POST", path, std::move(handler)});
}

void WebServer::put(const std::string& path, Handler handler) {
    routes_.push_back({"PUT", path, std::move(handler)});
}

void WebServer::del(const std::string& path, Handler handler) {
    routes_.push_back({"DELETE", path, std::move(handler)});
}

void WebServer::use(Middleware middleware) {
    middleware_.push_back(std::move(middleware));
}

void WebServer::serveStatic(const std::string& path, const std::string& dir) {
    staticFiles_[path] = dir;
}

void WebServer::start() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        throw std::runtime_error("Failed to bind");
    }

    if (listen(fd, 10) < 0) {
        close(fd);
        throw std::runtime_error("Failed to listen");
    }

    try {
        // The flag goes up before the socket is published, so that a stop()
        // that runs in between sees the flag down and the accept loop below
        // never starts.
        running_ = true;

        // The socket is published under the lock, so that stop() can shut it
        // down. It stays owned by this function: this is the only place
        // that closes it.
        {
            std::lock_guard<std::mutex> lock(mutex_);
            server_fd_ = fd;
        }

        std::cout << "Web server started on port " << port_ << "\n";

        while (running_) {
            int client_fd = accept(fd, nullptr, nullptr);
            if (client_fd < 0) {
                if (!running_) break;
                // The socket is gone. Retrying would spin on the error.
                if (errno == EBADF || errno == EINVAL || errno == ENOTSOCK) break;
                continue;
            }

            // The running flag and the thread list are read under the same
            // lock. A client that is accepted at the very moment the server
            // is stopped would otherwise end up in a list that nobody joins.
            bool spawned = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (running_) {
                    clientThreads_.emplace_back(&WebServer::handleClient, this, client_fd);
                    spawned = true;
                }
            }
            if (!spawned) {
                close(client_fd);
                break;
            }
        }
    } catch (...) {
        // The socket is forgotten before it is closed, so that a stop() that
        // runs at the same time cannot shut down a descriptor that the
        // system has already handed out to somebody else.
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (server_fd_ == fd) server_fd_ = -1;
        }
        close(fd);
        throw;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (server_fd_ == fd) server_fd_ = -1;
    }
    close(fd);
}

void WebServer::stop() {
    running_ = false;

    // shutdown() before close(): on Linux, close() does not wake a thread
    // that is already blocked in accept() or in recv(), so the accept loop
    // would never observe running_ == false and the join below would hang
    // forever. The socket itself has to be shut down.
    //
    // The shutdown happens under the lock. A descriptor that is closed while
    // another thread still holds its number may already belong to somebody
    // else by then.
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (server_fd_ >= 0) {
            shutdown(server_fd_, SHUT_RDWR);
        }
        for (int fd : clientFds_) {
            shutdown(fd, SHUT_RDWR);
        }
    }

    // The client threads are taken out of the list under the lock and joined
    // outside of it.
    for (;;) {
        std::vector<std::thread> threads;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (clientThreads_.empty()) break;
            threads.swap(clientThreads_);
        }
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
}

void WebServer::handleClient(int client_fd) {
    // Register the socket so stop() can wake this thread if it is blocked.
    // The check and the registration are one step, so that a connection
    // that arrives while the server is being stopped is closed instead of
    // being left to block in recv() forever.
    bool registered = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) {
            clientFds_.insert(client_fd);
            registered = true;
        }
    }
    if (!registered) {
        close(client_fd);
        return;
    }

    char buffer[4096];
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            clientFds_.erase(client_fd);
        }
        close(client_fd);
        return;
    }

    buffer[bytes] = '\0';
    Request request;
    if (!request.parse(buffer)) {
        auto response = Response::badRequest();
        std::string serialized = response.serialize();
        send(client_fd, serialized.c_str(), serialized.size(), 0);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            clientFds_.erase(client_fd);
        }
        close(client_fd);
        return;
    }

    Response response = handleRequest(request);
    std::string serialized = response.serialize();
    send(client_fd, serialized.c_str(), serialized.size(), 0);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        clientFds_.erase(client_fd);
    }
    close(client_fd);
}

Response WebServer::handleRequest(const Request& request) {
    for (auto& middleware : middleware_) {
        Response response;
        if (!middleware(const_cast<Request&>(request), response)) {
            return response;
        }
    }

    return routeRequest(request);
}

Response WebServer::routeRequest(const Request& request) {
    for (const auto& route : routes_) {
        if (route.method == request.method && route.path == request.path) {
            return route.handler(request);
        }
    }

    for (const auto& [path, dir] : staticFiles_) {
        if (request.path.substr(0, path.size()) == path) {
            std::string filePath = dir + request.path.substr(path.size());
            std::ifstream file(filePath);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
                Response resp;
                resp.statusCode = 200;
                resp.statusMessage = "OK";
                resp.headers["Content-Type"] = "text/plain";
                resp.headers["Content-Length"] = std::to_string(content.size());
                resp.body = content;
                return resp;
            }
        }
    }

    return Response::notFound();
}

bool WebServer::isRunning() const {
    return running_;
}

} // namespace web_server
