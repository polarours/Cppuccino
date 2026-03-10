#pragma once

#include <string>

namespace tiny_http_server {

class Router {
public:
    std::string route(const std::string& path);

};

}