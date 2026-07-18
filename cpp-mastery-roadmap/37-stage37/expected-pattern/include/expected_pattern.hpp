#pragma once

#include <string>
#include <variant>
#include <stdexcept>
#include <functional>
#include <iostream>

namespace expected_pattern {

template <typename T, typename E>
class Expected {
public:
    static Expected success(T value) {
        Expected exp;
        exp.data_ = std::move(value);
        exp.hasValue_ = true;
        return exp;
    }

    static Expected error(E err) {
        Expected exp;
        exp.err_ = std::move(err);
        exp.hasValue_ = false;
        return exp;
    }

    bool hasValue() const { return hasValue_; }
    explicit operator bool() const { return hasValue_; }

    const T& value() const & {
        if (!hasValue_) throw std::runtime_error("Accessed value on error");
        return data_;
    }

    T&& value() && {
        if (!hasValue_) throw std::runtime_error("Accessed value on error");
        return std::move(data_);
    }

    const E& error() const & {
        if (hasValue_) throw std::runtime_error("Accessed error on success");
        return err_;
    }

    template <typename F>
    auto map(F func) -> Expected<decltype(func(std::declval<T>())), E> {
        if (hasValue_) {
            return Expected<decltype(func(std::declval<T>())), E>::success(func(data_));
        }
        return Expected<decltype(func(std::declval<T>())), E>::error(err_);
    }

    template <typename F>
    auto andThen(F func) -> decltype(func(std::declval<T>())) {
        if (hasValue_) {
            return func(data_);
        }
        return decltype(func(std::declval<T>()))::error(err_);
    }

private:
    bool hasValue_ = false;
    T data_{};
    E err_{};
};

inline Expected<std::string, std::string> readFile(const std::string& path) {
    if (path.empty()) return Expected<std::string, std::string>::error("empty path");
    if (path.find(".txt") == std::string::npos) return Expected<std::string, std::string>::error("not a .txt file");
    return Expected<std::string, std::string>::success("content of " + path);
}

inline Expected<int, std::string> divide(int a, int b) {
    if (b == 0) return Expected<int, std::string>::error("division by zero");
    return Expected<int, std::string>::success(a / b);
}

} // namespace expected_pattern
