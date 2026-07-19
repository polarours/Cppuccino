#pragma once

#include "request.hpp"
#include "response.hpp"
#include <memory>
#include <functional>
#include <iostream>

namespace mini_http {

using HandlerFunc = std::function<Response(const Request&)>;

class Handler {
public:
    Handler() = default;

    template <typename F>
    Handler(F func) : impl_(std::make_shared<Impl<F>>(std::move(func))) {}

    Response handle(const Request& req) const {
        if (impl_) return impl_->handle(req);
        return Response::notFound();
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual Response handle(const Request& req) const = 0;
    };

    template <typename F>
    struct Impl : Concept {
        explicit Impl(F f) : func_(std::move(f)) {}
        Response handle(const Request& req) const override { return func_(req); }
        F func_;
    };

    std::shared_ptr<Concept> impl_;
};

} // namespace mini_http
