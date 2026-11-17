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

coroutine_pattern::Generator<long long> fibonacci(long long max) {
    // The numbers grow past what an int can hold at the 47th one, so
    // they are counted in something bigger.
    long long a = 0, b = 1;
    while (a <= max) {
        co_yield a;
        const long long next = a + b;
        a = b;
        b = next;
    }
}

// Regression: the numbers were counted in an int, and the 47th
// Fibonacci number does not fit in one. The signed overflow was undefined
// behaviour, and the last number came out wrong. The argument here is an
// upper bound, not a count, so it has to be the value itself.
void test_fibonacci_past_what_an_int_can_hold() {
    auto gen = fibonacci(2971215073LL);
    long long last = 0;
    std::size_t count = 0;
    while (gen.next()) {
        last = gen.value();
        ++count;
    }

    expect(count == 48, "expected 48 numbers, got " + std::to_string(count));
    expect(last == 2971215073LL,
           "expected 2971215073, got " + std::to_string(last));
}

void test_fibonacci() {
    auto gen = fibonacci(10);
    std::vector<long long> results;
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

// Regression: a move constructor takes the move assignment away, and the
// copy assignment is deleted, so a generator could not be given to
// another variable at all.
void test_generator_can_be_moved_into() {
    auto first = countTo(3);
    auto second = std::move(first);

    expect(second.next(), "expected the second generator to work");
    expect(second.value() == 1, "expected 1");

    // Moving into a generator that already holds one has to destroy the
    // coroutine that was there, or it is never destroyed at all.
    auto third = countTo(2);
    auto fourth = countTo(5);
    fourth = std::move(third);

    expect(fourth.next(), "expected the fourth generator to work");
    expect(fourth.value() == 1, "expected 1");
}

// A generator that was moved from holds nothing. Asking it for its value
// used to reach into a handle that was not there.
void test_a_generator_that_was_moved_from_says_so() {
    auto first = countTo(2);
    auto second = std::move(first);

    bool threw = false;
    try {
        (void)first.value();
    } catch (const std::logic_error&) {
        threw = true;
    }
    expect(threw, "expected the moved-from generator to say so");
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

        test_generator_can_be_moved_into();
        std::cout << "  test_generator_can_be_moved_into: PASS\n";

        test_a_generator_that_was_moved_from_says_so();
        std::cout << "  test_a_generator_that_was_moved_from_says_so: PASS\n";

        test_fibonacci_past_what_an_int_can_hold();
        std::cout << "  test_fibonacci_past_what_an_int_can_hold: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "coroutine_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\ncoroutine_pattern_tests passed" << std::endl;
    return 0;
}
