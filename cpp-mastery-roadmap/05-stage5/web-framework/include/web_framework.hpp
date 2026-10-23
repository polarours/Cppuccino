#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <sstream>
#include <atomic>
#include <mutex>

namespace web_framework {

struct Request {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string getHeader(const std::string& name) const;
    std::string getParam(const std::string& name) const;
};

struct Response {
    int statusCode = 200;
    std::string statusMessage = "OK";
    std::map<std::string, std::string> headers;
    std::string body;

    Response& setStatus(int code, const std::string& message);
    Response& setHeader(const std::string& name, const std::string& value);
    Response& setBody(const std::string& content);
    Response& setJson(const std::string& json);
    Response& setHtml(const std::string& html);

    std::string serialize() const;

    static Response ok(const std::string& body = "");
    static Response json(const std::string& json);
    static Response html(const std::string& html);
    static Response notFound();
    static Response badRequest(const std::string& error = "");
    static Response internalError(const std::string& error = "");
};

using Handler = std::function<Response(const Request&)>;
using Middleware = std::function<bool(Request&, Response&)>;

class Router {
public:
    void get(const std::string& path, Handler handler);
    void post(const std::string& path, Handler handler);
    void put(const std::string& path, Handler handler);
    void del(const std::string& path, Handler handler);

    Response route(const Request& request);

private:
    struct Route {
        std::string method;
        std::string path;
        Handler handler;
    };

    std::vector<Route> routes_;
};

class App {
public:
    App();
    ~App();

    App& get(const std::string& path, Handler handler);
    App& post(const std::string& path, Handler handler);
    App& put(const std::string& path, Handler handler);
    App& del(const std::string& path, Handler handler);

    App& use(Middleware middleware);

    Response handleRequest(const Request& request);

    void listen(int port);
    void stop();

private:
    Router router_;
    std::vector<Middleware> middleware_;
    int port_;
    // The listening socket. listen() creates it and closes it again when the
    // accept loop ends; stop() only shuts it down, so that a thread blocked
    // in accept() wakes up. Every access is under mutex_.
    int server_fd_;
    // The client the accept loop is serving right now. It is shut down as
    // well, so that a client that connects and says nothing cannot hold the
    // loop in recv() for ever.
    int client_fd_;
    std::atomic<bool> running_;
    std::mutex mutex_;
};

} // namespace web_framework
