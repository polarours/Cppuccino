#include "coroutine_pattern.hpp"

#include <iostream>

using namespace coroutine_pattern;

Generator<int> countTo(int n) {
    for (int i = 1; i <= n; ++i) {
        co_yield i;
    }
}

Generator<int> fibonacci(int max) {
    int a = 0, b = 1;
    while (a <= max) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    std::cout << "=== Coroutine Pattern Demo ===\n\n";

    auto gen = countTo(5);
    while (gen.next()) {
        std::cout << gen.value() << " ";
    }
    std::cout << "\n";

    auto fib = fibonacci(20);
    std::cout << "Fibonacci: ";
    while (fib.next()) {
        std::cout << fib.value() << " ";
    }
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
