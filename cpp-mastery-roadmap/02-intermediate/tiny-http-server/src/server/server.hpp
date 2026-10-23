#pragma once

#include "router/router.hpp"
#include "thread-pool/thread_pool.hpp"

#include <cstddef>
#include <string>
#include <atomic>
#include <mutex>
#include <set>

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
    // The listening socket. start() creates it and closes it again when the
    // accept loop ends; stop() only shuts it down, so that a thread blocked
    // in accept() wakes up. Every access is under mutex_.
    int server_fd_;
    Router router_;
    ThreadPool thread_pool_;
    std::atomic<bool> running_;
    // The client sockets that are still being served. stop() shuts them down
    // so that a worker blocked in read() wakes up instead of hanging the
    // join of the pool.
    std::set<int> clientFds_;
    std::mutex mutex_;

    void handle_client(int client_fd);
    void send_response(int client_fd, const HttpResponse& response);
    int setup_socket();
};

} // namespace tiny_http_server
