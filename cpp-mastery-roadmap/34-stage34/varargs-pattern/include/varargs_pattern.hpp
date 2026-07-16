#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace varargs_pattern {

template <typename... Args>
std::string format(const std::string& fmt, Args... args) {
    std::ostringstream oss;
    std::vector<std::string> parts = {args...};
    std::size_t pos = 0;
    std::size_t argIdx = 0;

    while (pos < fmt.size()) {
        auto placeholder = fmt.find("{}", pos);
        if (placeholder == std::string::npos) {
            oss << fmt.substr(pos);
            break;
        }
        oss << fmt.substr(pos, placeholder - pos);
        if (argIdx < parts.size()) {
            oss << parts[argIdx++];
        }
        pos = placeholder + 2;
    }
    return oss.str();
}

template <typename... Args>
void print(Args... args) {
    ((std::cout << args), ...);
    std::cout << "\n";
}

template <typename... Args>
std::vector<std::string> makeVector(Args... args) {
    std::vector<std::string> result;
    (result.push_back(std::to_string(args)), ...);
    return result;
}

} // namespace varargs_pattern
