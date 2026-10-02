#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace http_router {

struct Request {
    std::string method;   // "GET", "POST", ...
    std::string path;     // "/users/42"
};

struct Response {
    int status = 404;
    std::string body;
};

using Handler = std::function<Response(const Request&, const std::map<std::string, std::string>&)>;

// Route table with ":param" path segments:
//   add("GET", "/users/:id", handler)
//   match({{"GET", "/users/42"}}) -> handler + extracted params {{"id","42"}}
class Router {
public:
    void add(const std::string& method, const std::string& pattern, Handler handler);
    // Returns false when no route matches (method+path).
    bool match(const Request& req, Handler& out,
               std::map<std::string, std::string>& params) const;
    // Convenience: dispatch, or a 404 response when nothing matches.
    Response dispatch(const Request& req) const;
    std::size_t routeCount() const { return routes_.size(); }

private:
    struct Route {
        std::string method;
        std::vector<std::string> segments;  // pattern split by '/'
        Handler handler;
    };
    std::vector<Route> routes_;
};

}  // namespace http_router
