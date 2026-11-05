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
    std::lock_guard<std::mutex> lock(mutex_);
    routes_.push_back({method, splitPath(pattern), std::move(handler)});
}

bool Router::match(const Request& req, Handler& out,
                   std::map<std::string, std::string>& params) const {
    auto pathSegs = splitPath(req.path);
    Handler found;
    std::map<std::string, std::string> matched;

    {
        // The table is searched under the lock, because a route may be
        // added while another thread is matching a request.
        std::lock_guard<std::mutex> lock(mutex_);
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
                found = route.handler;
                matched = std::move(candidate);
                break;
            }
        }
    }

    if (!found) return false;
    out = std::move(found);
    params = std::move(matched);
    return true;
}

std::size_t Router::routeCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return routes_.size();
}

Response Router::dispatch(const Request& req) const {
    Handler h;
    std::map<std::string, std::string> params;
    if (match(req, h, params)) return h(req, params);
    return {404, "not found"};
}

}  // namespace http_router
