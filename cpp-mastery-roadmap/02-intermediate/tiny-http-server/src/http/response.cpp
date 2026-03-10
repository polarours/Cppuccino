#include "response.hpp"

namespace tiny_http_server {

HttpResponse::HttpResponse()
    : status_code_(200)
    , status_message_("OK") 
    {

    }    

void HttpResponse::set_status(int code, const std::string& message) {
    status_code_ = code;
    status_message_ = message;
}

void HttpResponse::set_header(const std::string& key, const std::string& value) {
    headers_[key] = value;
}

void HttpResponse::set_body(const std::string& body) {
    body_ = body;
    headers_["Content-Length"] = std::to_string(body_.size());
}

std::string HttpResponse::to_string() const {

    std::string response;
    response += "HTTP/1.1 "
             + std::to_string(status_code_)
             + " "
             + status_message_
             + "\r\n";

    for (const auto& header : headers_) {
        response += header.first + ": " + header.second + "\r\n";
    }

    response += "\r\n";
    response += body_;

    return response;
}

}