#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>

namespace middleware_pipeline {

struct Request {
    std::string path;
    std::string method;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct Response {
    int status = 200;
    std::string body;
    std::map<std::string, std::string> headers;
};

using Middleware = std::function<void(Request&, Response&, std::function<void()>)>;

class Pipeline {
public:
    void use(Middleware middleware) {
        middlewares_.push_back(std::move(middleware));
    }

    void execute(Request& req, Response& res) {
        index_ = 0;
        currentReq_ = &req;
        currentRes_ = &res;
        next();
    }

private:
    void next() {
        if (index_ < middlewares_.size()) {
            auto idx = index_++;
            middlewares_[idx](*currentReq_, *currentRes_, [this]() { next(); });
        }
    }

    std::vector<Middleware> middlewares_;
    std::size_t index_ = 0;
    Request* currentReq_ = nullptr;
    Response* currentRes_ = nullptr;
};

} // namespace middleware_pipeline
