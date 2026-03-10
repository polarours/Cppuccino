#pragma once

#include <string>
#include <unordered_map>

namespace tiny_http_server {

class HttpResponse {
public:
    HttpResponse();
    std::string to_string() const;
    
    void set_status(int code, const std::string& message);
    void set_header(const std::string& key, const std::string& value);
    void set_body(const std::string& body);

private:
    int status_code_ {};
    std::string status_message_ {};
    std::unordered_map<std::string, std::string> headers_;
    std::string body_ {};    
};

}