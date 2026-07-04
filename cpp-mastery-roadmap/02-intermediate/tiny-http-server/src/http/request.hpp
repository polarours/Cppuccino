#pragma once

#include <string>
#include <unordered_map>

namespace tiny_http_server {

class HttpRequest {
public:
    HttpRequest() = default;
    
    bool parse(const std::string& raw);
    const std::string& method() const { return method_; }
    const std::string& path() const { return path_; }
    const std::string& version() const { return version_; }
    std::string header(const std::string& key) const;

private:
    std::string method_;
    std::string path_;
    std::string version_;
    std::unordered_map<std::string, std::string> headers_;
};

}
