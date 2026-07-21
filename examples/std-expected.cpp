#include <expected>
#include <string>
#include <iostream>
#include <stdexcept>

// std::expected - value or error (C++23)
// <expected>

enum class ErrorCode {
    None,
    NotFound,
    InvalidInput,
    NetworkError
};

std::string errorMessage(ErrorCode code) {
    switch (code) {
        case ErrorCode::NotFound: return "Not found";
        case ErrorCode::InvalidInput: return "Invalid input";
        case ErrorCode::NetworkError: return "Network error";
        default: return "No error";
    }
}

std::expected<int, ErrorCode> parseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::unexpected(ErrorCode::InvalidInput);
    }
}

std::expected<double, ErrorCode> divide(double a, double b) {
    if (b == 0) return std::unexpected(ErrorCode::InvalidInput);
    return a / b;
}

std::expected<std::string, ErrorCode> readConfig(const std::string& key) {
    if (key == "host") return std::string("localhost");
    if (key == "port") return std::string("8080");
    return std::unexpected(ErrorCode::NotFound);
}

int main() {
    std::cout << "=== std::expected (C++23) ===\n\n";

    // Basic usage
    auto result = parseInt("42");
    if (result) {
        std::cout << "Parsed: " << *result << "\n";
    }

    auto error = parseInt("abc");
    if (!error) {
        std::cout << "Error: " << errorMessage(error.error()) << "\n";
    }

    // value_or
    auto val = parseInt("xyz").value_or(0);
    std::cout << "value_or(0): " << val << "\n";  // 0

    // Monadic operations
    auto divideResult = divide(10.0, 3.0);
    if (divideResult) {
        std::cout << "\n10/3 = " << *divideResult << "\n";
    }

    // Transform with and_then
    auto transformed = parseInt("42")
        .and_then([](int v) -> std::expected<int, ErrorCode> {
            return v * 2;
        });
    if (transformed) {
        std::cout << "Transformed: " << *transformed << "\n";  // 84
    }

    // Error propagation
    auto config = readConfig("host");
    if (config) {
        std::cout << "\nhost = " << *config << "\n";
    }

    auto missing = readConfig("unknown");
    if (!missing) {
        std::cout << "Config error: " << errorMessage(missing.error()) << "\n";
    }

    // Chaining with and_then
    auto chained = parseInt("10")
        .and_then([](int v) { return divide(v, 2.0); });

    if (chained) {
        std::cout << "\nChained: " << *chained << "\n";  // 5.0
    }

    return 0;
}
