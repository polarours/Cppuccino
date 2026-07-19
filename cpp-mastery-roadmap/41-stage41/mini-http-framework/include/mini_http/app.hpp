#pragma once

#include "request.hpp"
#include "response.hpp"
#include "handler.hpp"
#include "router.hpp"
#include "middleware.hpp"
#include "event_bus.hpp"
#include <string>
#include <iostream>

namespace mini_http {

class App {
public:
    static App& instance() {
        static App app;
        return app;
    }

    Router& router() { return router_; }
    MiddlewarePipeline& pipeline() { return pipeline_; }
    EventBus& events() { return events_; }

    void get(const std::string& path, HandlerFunc handler) {
        router_.get(path, Handler(std::move(handler)));
    }

    void post(const std::string& path, HandlerFunc handler) {
        router_.post(path, Handler(std::move(handler)));
    }

    void use(MiddlewareFunc middleware) {
        pipeline_.use(std::move(middleware));
    }

    void on(const std::string& event, EventHandler handler) {
        events_.subscribe(event, std::move(handler));
    }

    Response handle(const Request& req) {
        events_.publish({"request.received", req.method() + " " + req.path()});
        Request mutableReq = req;
        auto res = pipeline_.execute(mutableReq);
        if (res.statusCode() == 200 && res.body().empty()) {
            res = router_.dispatch(mutableReq);
        }
        events_.publish({"response.sent", std::to_string(res.statusCode())});
        return res;
    }

    Response handleRaw(const std::string& rawRequest) {
        return handle(Request::parse(rawRequest));
    }

    std::string serve() const {
        std::ostringstream oss;
        oss << "MiniHTTP Framework v1.0\n";
        oss << "Routes: " << router_.routeCount() << "\n";
        oss << "Middlewares: " << pipeline_.count() << "\n";
        return oss.str();
    }

private:
    App() = default;
    Router router_;
    MiddlewarePipeline pipeline_;
    EventBus events_;
};

} // namespace mini_http
