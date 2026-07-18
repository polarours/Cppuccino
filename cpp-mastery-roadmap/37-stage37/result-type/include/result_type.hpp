#pragma once

#include <string>
#include <variant>
#include <stdexcept>
#include <iostream>

namespace result_type {

class Error {
public:
    Error(std::string message) : message_(std::move(message)) {}
    const std::string& message() const { return message_; }
private:
    std::string message_;
};

template <typename T>
class Result {
public:
    static Result ok(T value) {
        Result r;
        r.data_ = std::move(value);
        r.ok_ = true;
        return r;
    }

    static Result fail(std::string message) {
        Result r;
        r.error_ = Error(std::move(message));
        r.ok_ = false;
        return r;
    }

    bool isSuccess() const { return ok_; }
    bool isFailure() const { return !ok_; }

    const T& value() const {
        if (!ok_) throw std::runtime_error("Accessed value on error result");
        return data_;
    }

    const Error& error() const {
        if (ok_) throw std::runtime_error("Accessed error on success result");
        return error_;
    }

    T valueOr(const T& defaultValue) const {
        return ok_ ? data_ : defaultValue;
    }

private:
    Result() = default;
    bool ok_ = false;
    T data_{};
    Error error_{""};
};

inline Result<int> parseInt(const std::string& str) {
    try {
        return Result<int>::ok(std::stoi(str));
    } catch (...) {
        return Result<int>::fail("Invalid integer: " + str);
    }
}

} // namespace result_type
