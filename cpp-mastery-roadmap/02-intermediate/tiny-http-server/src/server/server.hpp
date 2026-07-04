#pragma once

#include "router/router.hpp"
#include "thread-pool/thread_pool.hpp"

#include <cstddef>
#include <string>

namespace tiny_http_server {

class Server {
public:
    Server(const std::string& addr, int port, std::size_t thread_count = 4);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void start();
    void stop();

    void addRoute(const std::string& method, const std::string& path, RouteHandler handler);

private:
    int port_;
    std::string addr_;
    int server_fd_;
    Router router_;
    ThreadPool thread_pool_;
    bool running_;

    void handle_client(int client_fd);
    void send_response(int client_fd, const HttpResponse& response);
    void setup_socket();
    void cleanup();
};

} // namespace tiny_http_server
