#include "ranges_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== Ranges Pattern Demo ===\n\n";

    ranges_pattern::Range range({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    auto result = range.filter([](int x) { return x % 2 == 0; })
                      .map([](int x) { return x * x; })
                      .take(3);

    std::cout << "Even squares (first 3): ";
    for (int v : result.data()) std::cout << v << " ";
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
