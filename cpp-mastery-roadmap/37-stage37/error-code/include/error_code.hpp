#pragma once

#include <string>
#include <map>
#include <iostream>

namespace error_code {

enum class ErrorCode {
    None = 0,
    NotFound = 1,
    InvalidArgument = 2,
    PermissionDenied = 3,
    InternalError = 4,
    Timeout = 5
};

inline std::string errorMessage(ErrorCode code) {
    static const std::map<ErrorCode, std::string> messages = {
        {ErrorCode::None, "No error"},
        {ErrorCode::NotFound, "Resource not found"},
        {ErrorCode::InvalidArgument, "Invalid argument"},
        {ErrorCode::PermissionDenied, "Permission denied"},
        {ErrorCode::InternalError, "Internal error"},
        {ErrorCode::Timeout, "Operation timed out"}
    };
    auto it = messages.find(code);
    return (it != messages.end()) ? it->second : "Unknown error";
}

class OperationResult {
public:
    OperationResult() = default;
    OperationResult(ErrorCode code) : code_(code) {}
    OperationResult(ErrorCode code, std::string detail)
        : code_(code), detail_(std::move(detail)) {}

    bool ok() const { return code_ == ErrorCode::None; }
    ErrorCode code() const { return code_; }
    std::string message() const {
        return errorMessage(code_) + (detail_.empty() ? "" : ": " + detail_);
    }
    const std::string& detail() const { return detail_; }

private:
    ErrorCode code_ = ErrorCode::None;
    std::string detail_;
};

class UserService {
public:
    OperationResult deleteUser(int id) {
        if (id <= 0) return OperationResult(ErrorCode::InvalidArgument, "id must be positive");
        if (users_.find(id) == users_.end()) return OperationResult(ErrorCode::NotFound, "user " + std::to_string(id));
        users_.erase(id);
        return OperationResult(ErrorCode::None);
    }

    void addUser(int id, const std::string& name) {
        users_[id] = name;
    }

    std::size_t userCount() const { return users_.size(); }

private:
    std::map<int, std::string> users_;
};

} // namespace error_code
