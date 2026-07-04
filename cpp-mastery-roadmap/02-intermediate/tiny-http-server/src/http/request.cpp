#include "request.hpp"

#include <sstream>
#include <string>

namespace tiny_http_server {

bool HttpRequest::parse(const std::string& raw) {
    method_.clear();
    path_.clear();
    version_.clear();
    headers_.clear();

    std::istringstream stream(raw);

    stream >> method_ >> path_ >> version_;
    if (method_.empty() || path_.empty() || version_.empty() ||
        version_.rfind("HTTP/", 0) != 0) {
        return false;
    }

    std::string line;
    std::getline(stream, line);

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            break;
        }

        auto colon_pos = line.find(':');

        if (colon_pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        while (!value.empty() && value[0] == ' ') {
            value.erase(0, 1);
        }

        headers_[key] = value;
    }

    return true;
}

std::string HttpRequest::header(const std::string& key) const {
    auto it = headers_.find(key);
    if (it != headers_.end()) {
        return it->second;
    }
    return "";
}

} // namespace tiny_http_server
