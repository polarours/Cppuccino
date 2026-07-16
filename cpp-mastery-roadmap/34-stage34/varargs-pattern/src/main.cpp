#include "varargs_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== Varargs Pattern Demo ===\n\n";

    std::cout << varargs_pattern::format("Hello, {}!", std::string("World")) << "\n";
    std::cout << varargs_pattern::format("{} is {} years old", std::string("Alice"), std::to_string(30)) << "\n";

    varargs_pattern::print("a", "b", "c");

    auto vec = varargs_pattern::makeVector(1, 2, 3);
    std::cout << "Vector: ";
    for (const auto& v : vec) std::cout << v << " ";
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
