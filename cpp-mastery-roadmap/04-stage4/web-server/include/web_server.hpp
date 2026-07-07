#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>

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
    int server_fd_;
    bool running_;

    struct Route {
        std::string method;
        std::string path;
        Handler handler;
    };

    std::vector<Route> routes_;
    std::vector<Middleware> middleware_;
    std::map<std::string, std::string> staticFiles_;
    std::vector<std::thread> clientThreads_;
    std::mutex mutex_;
};

} // namespace web_server
