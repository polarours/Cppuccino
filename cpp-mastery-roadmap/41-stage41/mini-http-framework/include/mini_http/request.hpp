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

        // The head and the body are separated by an empty line. The body
        // is taken from the raw text and not read line by line, so that
        // it keeps its line endings and its last newline: a server that
        // reads the body by its content length has to get exactly as
        // many bytes as the header says.
        std::string head = raw;
        std::size_t bodyStart = std::string::npos;
        const std::size_t crlf = raw.find("\r\n\r\n");
        const std::size_t lf = raw.find("\n\n");
        if (crlf != std::string::npos && (lf == std::string::npos || crlf <= lf)) {
            head = raw.substr(0, crlf);
            bodyStart = crlf + 4;
        } else if (lf != std::string::npos) {
            head = raw.substr(0, lf);
            bodyStart = lf + 2;
        }

        std::istringstream stream(head);
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
                std::string key = trim(line.substr(0, colon));
                // The space behind the colon is optional, and there may
                // be more of it than one. Reading a fixed two characters
                // ate the first one of the value when there was no
                // space at all.
                std::string val = trim(line.substr(colon + 1));
                req.headers_[key] = val;
            }
        }

        if (bodyStart != std::string::npos) {
            req.body_ = raw.substr(bodyStart);
        }

        return req;
    }

private:
    static std::string trim(const std::string& text) {
        const char* ws = " \t\r\n";
        const auto begin = text.find_first_not_of(ws);
        if (begin == std::string::npos) return "";
        const auto end = text.find_last_not_of(ws);
        return text.substr(begin, end - begin + 1);
    }

    std::string method_ = "GET";
    std::string path_ = "/";
    std::string body_;
    std::map<std::string, std::string> headers_;
    std::map<std::string, std::string> queryParams_;
};

} // namespace mini_http
