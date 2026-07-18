#include "coroutine_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

coroutine_pattern::Generator<int> countTo(int n) {
    for (int i = 1; i <= n; ++i) {
        co_yield i;
    }
}

void test_generator_basic() {
    auto gen = countTo(3);

    expect(gen.next(), "expected next");
    expect(gen.value() == 1, "expected 1");

    expect(gen.next(), "expected next");
    expect(gen.value() == 2, "expected 2");

    expect(gen.next(), "expected next");
    expect(gen.value() == 3, "expected 3");

    expect(!gen.next(), "expected done");
}

coroutine_pattern::Generator<int> fibonacci(int max) {
    int a = 0, b = 1;
    while (a <= max) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

void test_fibonacci() {
    auto gen = fibonacci(10);
    std::vector<int> results;
    while (gen.next()) {
        results.push_back(gen.value());
    }
    expect(results.size() == 7, "expected 7 fibonacci numbers <= 10");
    expect(results[0] == 0, "expected 0");
    expect(results[1] == 1, "expected 1");
    expect(results[6] == 8, "expected 8");
}

coroutine_pattern::Generator<std::string> letters() {
    co_yield "A";
    co_yield "B";
    co_yield "C";
}

void test_string_generator() {
    auto gen = letters();
    expect(gen.next() && gen.value() == "A", "expected A");
    expect(gen.next() && gen.value() == "B", "expected B");
    expect(gen.next() && gen.value() == "C", "expected C");
    expect(!gen.next(), "expected done");
}

} // namespace

int main() {
    try {
        std::cout << "Coroutine Pattern Tests:\n";
        test_generator_basic();
        std::cout << "  test_generator_basic: PASS\n";

        test_fibonacci();
        std::cout << "  test_fibonacci: PASS\n";

        test_string_generator();
        std::cout << "  test_string_generator: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "coroutine_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncoroutine_pattern_tests passed" << std::endl;
    return 0;
}
