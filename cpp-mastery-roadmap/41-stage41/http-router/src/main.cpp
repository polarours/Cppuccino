#include "http_router.hpp"

#include <iostream>

int main() {
    http_router::Router router;
    router.add("GET", "/", [](const auto&, const auto&) {
        return http_router::Response{200, "home"};
    });
    router.add("GET", "/users/:id", [](const auto&, const auto& p) {
        return http_router::Response{200, "user " + p.at("id")};
    });
    router.add("GET", "/users/:id/posts/:pid", [](const auto&, const auto& p) {
        return http_router::Response{200, "post " + p.at("pid") + " by " + p.at("id")};
    });

    for (auto& req : {http_router::Request{"GET", "/"},
                      http_router::Request{"GET", "/users/42"},
                      http_router::Request{"GET", "/users/7/posts/9"},
                      http_router::Request{"POST", "/users/1"}}) {
        auto resp = router.dispatch(req);
        std::cout << req.method << " " << req.path << " -> "
                  << resp.status << " " << resp.body << "\n";
    }
    return 0;
}
