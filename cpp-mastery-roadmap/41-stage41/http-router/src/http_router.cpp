#include "http_router.hpp"

#include <sstream>

namespace http_router {

namespace {

std::vector<std::string> splitPath(const std::string& path) {
    std::vector<std::string> segs;
    std::istringstream in(path);
    std::string seg;
    while (std::getline(in, seg, '/'))
        if (!seg.empty()) segs.push_back(seg);
    return segs;
}

}  // namespace

void Router::add(const std::string& method, const std::string& pattern, Handler handler) {
    routes_.push_back({method, splitPath(pattern), std::move(handler)});
}

bool Router::match(const Request& req, Handler& out,
                   std::map<std::string, std::string>& params) const {
    auto pathSegs = splitPath(req.path);
    for (const auto& route : routes_) {
        if (route.method != req.method) continue;
        if (route.segments.size() != pathSegs.size()) continue;
        std::map<std::string, std::string> candidate;
        bool ok = true;
        for (std::size_t i = 0; i < route.segments.size(); ++i) {
            const auto& pat = route.segments[i];
            if (!pat.empty() && pat[0] == ':') {
                candidate[pat.substr(1)] = pathSegs[i];
            } else if (pat != pathSegs[i]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            out = route.handler;
            params = std::move(candidate);
            return true;
        }
    }
    return false;
}

Response Router::dispatch(const Request& req) const {
    Handler h;
    std::map<std::string, std::string> params;
    if (match(req, h, params)) return h(req, params);
    return {404, "not found"};
}

}  // namespace http_router
