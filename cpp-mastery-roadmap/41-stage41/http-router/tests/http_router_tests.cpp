#include "http_router.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

http_router::Response ok200(const http_router::Request&,
                            const std::map<std::string, std::string>&) {
    return {200, "ok"};
}

void test_exact_match() {
    http_router::Router r;
    r.add("GET", "/", ok200);
    http_router::Handler h;
    std::map<std::string, std::string> params;
    expect(r.match({"GET", "/"}, h, params), "exact root matches");
    expect(!r.match({"POST", "/"}, h, params), "method must match");
}

void test_param_extraction() {
    http_router::Router r;
    r.add("GET", "/users/:id/posts/:pid",
          [](const auto&, const auto& p) {
              return http_router::Response{200, p.at("id") + "/" + p.at("pid")};
          });
    auto resp = r.dispatch({"GET", "/users/42/posts/7"});
    expect(resp.status == 200, "matched");
    expect(resp.body == "42/7", "both params extracted");
}

void test_no_match_404() {
    http_router::Router r;
    r.add("GET", "/", ok200);
    auto resp = r.dispatch({"GET", "/nope"});
    expect(resp.status == 404, "404 for unknown path");
    auto resp2 = r.dispatch({"DELETE", "/"});
    expect(resp2.status == 404, "404 for wrong method");
}

void test_segment_count_must_match() {
    http_router::Router r;
    r.add("GET", "/users/:id", ok200);
    auto resp = r.dispatch({"GET", "/users/42/extra"});
    expect(resp.status == 404, "extra segment does not match");
}

void test_literal_beats_nothing() {
    http_router::Router r;
    r.add("GET", "/users/me", ok200);
    http_router::Handler h;
    std::map<std::string, std::string> params;
    // literal route should not capture "me" as a param when only literal exists
    expect(r.match({"GET", "/users/me"}, h, params), "literal matches");
    expect(params.empty(), "literal route extracts no params");
}

}  // namespace

int main() {
    try {
        test_exact_match();
        test_param_extraction();
        test_no_match_404();
        test_segment_count_must_match();
        test_literal_beats_nothing();
        std::cout << "All http_router tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
