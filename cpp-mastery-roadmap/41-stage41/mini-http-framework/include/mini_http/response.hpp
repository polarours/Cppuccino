#pragma once

#include <string>
#include <map>
#include <sstream>
#include <iostream>

namespace mini_http {

class Response {
public:
    static Response ok(const std::string& body = "") {
        Response res;
        res.status_ = 200;
        res.statusText_ = "OK";
        res.body_ = body;
        return res;
    }

    static Response notFound() {
        Response res;
        res.status_ = 404;
        res.statusText_ = "Not Found";
        res.body_ = "Not Found";
        return res;
    }

    static Response badRequest(const std::string& msg = "Bad Request") {
        Response res;
        res.status_ = 400;
        res.statusText_ = "Bad Request";
        res.body_ = msg;
        return res;
    }

    static Response internalError(const std::string& msg = "Internal Error") {
        Response res;
        res.status_ = 500;
        res.statusText_ = "Internal Error";
        res.body_ = msg;
        return res;
    }

    Response& status(int code, const std::string& text) {
        status_ = code;
        statusText_ = text;
        return *this;
    }

    Response& header(const std::string& key, const std::string& value) {
        headers_[key] = value;
        return *this;
    }

    Response& body(const std::string& content) {
        body_ = content;
        headers_["Content-Length"] = std::to_string(body_.size());
        return *this;
    }

    Response& contentType(const std::string& type) {
        headers_["Content-Type"] = type;
        return *this;
    }

    int statusCode() const { return status_; }
    const std::string& statusText() const { return statusText_; }
    const std::string& body() const { return body_; }
    const std::map<std::string, std::string>& headers() const { return headers_; }
    std::string header(const std::string& key) const {
        auto it = headers_.find(key);
        return (it != headers_.end()) ? it->second : "";
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << "HTTP/1.1 " << status_ << " " << statusText_ << "\r\n";
        for (const auto& [k, v] : headers_) {
            oss << k << ": " << v << "\r\n";
        }
        oss << "\r\n" << body_;
        return oss.str();
    }

private:
    int status_ = 200;
    std::string statusText_ = "OK";
    std::string body_;
    std::map<std::string, std::string> headers_;
};

} // namespace mini_http
