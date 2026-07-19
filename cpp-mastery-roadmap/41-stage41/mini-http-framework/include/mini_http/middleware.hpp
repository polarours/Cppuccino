#pragma once

#include "request.hpp"
#include "response.hpp"
#include <vector>
#include <functional>
#include <iostream>

namespace mini_http {

class MiddlewarePipeline;

using MiddlewareFunc = std::function<void(const Request&, Response&, std::function<void()>)>;

class MiddlewarePipeline {
public:
    void use(MiddlewareFunc middleware) {
        middlewares_.push_back(std::move(middleware));
    }

    Response execute(Request& req) {
        Response res;
        index_ = 0;
        currentReq_ = &req;
        currentRes_ = &res;
        next();
        return res;
    }

    std::size_t count() const { return middlewares_.size(); }

private:
    void next() {
        if (index_ < middlewares_.size()) {
            auto idx = index_++;
            middlewares_[idx](*currentReq_, *currentRes_, [this]() { next(); });
        }
    }

    std::vector<MiddlewareFunc> middlewares_;
    std::size_t index_ = 0;
    Request* currentReq_ = nullptr;
    Response* currentRes_ = nullptr;
};

} // namespace mini_http
