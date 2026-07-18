#include "expected_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== Expected Pattern Demo ===\n\n";

    auto r1 = expected_pattern::divide(10, 2);
    if (r1) std::cout << "10/2 = " << r1.value() << "\n";

    auto r2 = expected_pattern::divide(10, 0);
    if (!r2) std::cout << "10/0: " << r2.error() << "\n";

    auto mapped = expected_pattern::divide(20, 4)
        .map([](int v) { return "Result: " + std::to_string(v); });
    if (mapped) std::cout << mapped.value() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
