#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

namespace web_server {

struct Request {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    bool parse(const std::string& raw);
};

struct Response {
    int statusCode = 200;
    std::string statusMessage = "OK";
    std::map<std::string, std::string> headers;
    std::string body;

    std::string serialize() const;

    static Response ok(const std::string& body = "");
    static Response notFound();
    static Response badRequest();
    static Response internalError();
};

using Handler = std::function<Response(const Request&)>;
using Middleware = std::function<bool(Request&, Response&)>;

class WebServer {
public:
    WebServer(int port);
    ~WebServer();

    void get(const std::string& path, Handler handler);
    void post(const std::string& path, Handler handler);
    void put(const std::string& path, Handler handler);
    void del(const std::string& path, Handler handler);

    void use(Middleware middleware);
    void serveStatic(const std::string& path, const std::string& dir);

    void start();
    void stop();

    bool isRunning() const;

private:
    void handleClient(int client_fd);
    Response handleRequest(const Request& request);
    Response routeRequest(const Request& request);

    int port_;
    // The listening socket. start() creates it and closes it again when the
    // accept loop ends; stop() only shuts it down, so that a thread blocked
    // in accept() wakes up. Every access is under mutex_.
    int server_fd_;
    std::atomic<bool> running_;

    struct Route {
        std::string method;
        std::string path;
        Handler handler;
    };

    std::vector<Route> routes_;
    std::vector<Middleware> middleware_;
    std::map<std::string, std::string> staticFiles_;
    // The threads that serve the clients. The accept loop fills the list
    // and stop() takes it over, both under mutex_. The threads themselves
    // are joined outside of the lock.
    std::vector<std::thread> clientThreads_;
    // The client sockets that are still being served. stop() shuts them down
    // so that a thread blocked in recv() wakes up instead of hanging the
    // join.
    std::set<int> clientFds_;
    std::mutex mutex_;
};

} // namespace web_server
