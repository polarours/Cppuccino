#pragma once

#include <string>
#include <map>
#include <sstream>
#include <iostream>

namespace mini_http {

class Request {
public:
    Request() = default;
    Request(std::string method, std::string path)
        : method_(std::move(method)), path_(std::move(path)) {}

    const std::string& method() const { return method_; }
    const std::string& path() const { return path_; }
    const std::string& body() const { return body_; }
    const std::map<std::string, std::string>& headers() const { return headers_; }
    const std::map<std::string, std::string>& queryParams() const { return queryParams_; }

    void setMethod(std::string m) { method_ = std::move(m); }
    void setPath(std::string p) { path_ = std::move(p); }
    void setBody(std::string b) { body_ = std::move(b); }
    void addHeader(const std::string& k, const std::string& v) { headers_[k] = v; }
    void addQueryParam(const std::string& k, const std::string& v) { queryParams_[k] = v; }

    std::string header(const std::string& key) const {
        auto it = headers_.find(key);
        return (it != headers_.end()) ? it->second : "";
    }

    static Request parse(const std::string& raw) {
        Request req;
        std::istringstream stream(raw);
        std::string line;

        if (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            auto sp1 = line.find(' ');
            auto sp2 = line.find(' ', sp1 + 1);
            if (sp1 != std::string::npos && sp2 != std::string::npos) {
                req.method_ = line.substr(0, sp1);
                req.path_ = line.substr(sp1 + 1, sp2 - sp1 - 1);
            }
        }

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) break;
            auto colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string val = line.substr(colon + 2);
                req.headers_[key] = val;
            }
        }

        std::string body;
        while (std::getline(stream, line)) body += line;
        req.body_ = body;

        return req;
    }

private:
    std::string method_ = "GET";
    std::string path_ = "/";
    std::string body_;
    std::map<std::string, std::string> headers_;
    std::map<std::string, std::string> queryParams_;
};

} // namespace mini_http
