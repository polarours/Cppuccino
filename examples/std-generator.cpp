// examples/std-generator.cpp
// Demonstrates C++23 std::generator: a coroutine-based lazy sequence without
// hand-writing promise_type (contrast with cpp20-coroutines-complete.cpp).
// Compile: g++ -std=c++23 -Wall -Wextra -o std-generator std-generator.cpp

#include <generator>
#include <iostream>
#include <string>
#include <vector>

namespace generator_demo {

// Fibonacci as a lazy coroutine sequence
std::generator<int> fibonacci(int n) {
    int a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

// Generator composing another generator (pipeline style)
std::generator<std::string> label(std::generator<int> nums, const std::string& fmt) {
    int index = 0;
    for (int v : nums) {
        co_yield std::to_string(index++) + ": " + std::to_string(v);
        // fmt kept simple to avoid <format> dependency in the signature demo
        (void)fmt;
    }
}

// Range-for over a generator is all the "API" you need
std::generator<int> squares(int limit) {
    for (int i = 1; i * i <= limit; ++i) co_yield i * i;
}

} // namespace generator_demo

int main() {
    using namespace generator_demo;
    std::cout << "=== std::generator (C++23) Demo ===\n\n";

    std::cout << "fibonacci(10): ";
    for (int v : fibonacci(10)) std::cout << v << ' ';
    std::cout << "\n\n";

    std::cout << "squares(50): ";
    for (int v : squares(50)) std::cout << v << ' ';
    std::cout << "\n\n";

    // Consumer controls iteration - nothing computed until looped
    std::cout << "composed generator (first 5 of fibonacci(8)):\n";
    int count = 0;
    for (const auto& line : label(fibonacci(8), "fib")) {
        std::cout << "  " << line << "\n";
        if (++count == 5) break;  // early exit: no further values are produced
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
