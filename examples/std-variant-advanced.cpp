#include <variant>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

// std::variant - type-safe union (C++17)
// <variant>

using Value = std::variant<int, double, std::string, bool>;

void printValue(const Value& v) {
    std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            std::cout << "int: " << val;
        } else if constexpr (std::is_same_v<T, double>) {
            std::cout << "double: " << val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "string: \"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            std::cout << "bool: " << std::boolalpha << val;
        }
    }, v);
    std::cout << "\n";
}

int main() {
    std::cout << "=== std::variant (C++17) ===\n\n";

    // Basic usage
    std::variant<int, double, std::string> v1 = 42;
    std::variant<int, double, std::string> v2 = 3.14;
    std::variant<int, double, std::string> v3 = "hello";

    std::cout << "v1: " << std::get<int>(v1) << "\n";
    std::cout << "v2: " << std::get<double>(v2) << "\n";
    std::cout << "v3: " << std::get<std::string>(v3) << "\n";

    // Type checking
    std::cout << "\nv1 is int: " << std::holds_alternative<int>(v1) << "\n";
    std::cout << "v1 is string: " << std::holds_alternative<std::string>(v1) << "\n";

    // std::get with index
    std::cout << "v2[1]: " << std::get<1>(v2) << "\n";

    // std::get with type
    std::cout << "v3 string: " << std::get<std::string>(v3) << "\n";

    // Exception on wrong type
    try {
        auto val = std::get<int>(v2);  // v2 is double, not int
    } catch (const std::bad_variant_access& e) {
        std::cout << "\nBad access: " << e.what() << "\n";
    }

    // std::visit with overloaded lambda
    std::cout << "\nVisiting values:\n";
    printValue(42);
    printValue(3.14);
    printValue(std::string("hello"));
    printValue(true);

    // Vector of variants
    std::vector<Value> values = {42, 3.14, std::string("hello"), true};

    std::cout << "\nVector of variants:\n";
    for (const auto& v : values) {
        std::cout << "  ";
        printValue(v);
    }

    // Structured bindings with variant
    std::variant<int, std::string> v4 = "test";
    if (auto* str = std::get_if<std::string>(&v4)) {
        std::cout << "\nget_if string: " << *str << "\n";
    }

    return 0;
}
