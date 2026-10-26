// examples/pitfall-signed-integer-overflow-assumptions.cpp
// Demonstrates that signed overflow is undefined behaviour and shows checked arithmetic patterns.
// Compile with: g++ -std=c++17 -o pitfall_signed_integer_overflow_assumptions pitfall-signed-integer-overflow-assumptions.cpp

#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

namespace {

// --- 1. The canonical misleading example ---

// At -O2 the optimizer may fold this into `return true;`, because the only
// input for which the comparison could be false is x == INT_MAX, and
// x + 1 would overflow there. That is undefined behaviour, so the optimizer
// is allowed to assume the case never happens.
int alwaysGreater(int x) {
    return x + 1 > x;
}

// --- 2. Checked arithmetic ---

bool add_checked(int a, int b, int& out) {
    if (b > 0 && a > std::numeric_limits<int>::max() - b) {
        return false;  // the mathematical sum does not fit
    }
    if (b < 0 && a < std::numeric_limits<int>::min() - b) {
        return false;
    }
    out = a + b;
    return true;
}

// Compute in a wider type, then validate before narrowing back.
bool sum_to_int(int a, int b, int& out) {
    std::int64_t wide = static_cast<std::int64_t>(a) + static_cast<std::int64_t>(b);
    if (wide < std::numeric_limits<int>::min() || wide > std::numeric_limits<int>::max()) {
        return false;
    }
    out = static_cast<int>(wide);
    return true;
}

bool mul_checked(std::size_t a, std::size_t b, std::size_t& out) {
    const std::size_t limit = std::numeric_limits<std::size_t>::max();
    if (a != 0 && b > limit / a) {
        return false;
    }
    out = a * b;
    return true;
}

}  // namespace

// --- 1. Signed overflow is undefined behaviour ---

void signedOverflowIsUndefined() {
    std::cout << "--- Signed overflow is undefined behaviour ---\n";

    // BAD: this call is undefined behaviour, so it is not executed here.
    // int result = alwaysGreater(std::numeric_limits<int>::max());
    // With -O2 the compiler may assume the overflowing case is impossible and
    // fold the function body to `return true;`.
    // Build with -fsanitize=signed-integer-overflow to catch it.

    std::cout << "alwaysGreater(1000) = " << alwaysGreater(1000) << "\n";

    // BAD: negating INT_MIN is undefined behaviour (the positive value does
    // not exist in int), so it is not executed here.
    // int negated = -std::numeric_limits<int>::min();

    // GOOD: handle the one value that has no positive counterpart.
    int x = std::numeric_limits<int>::min();
    int magnitude = (x == std::numeric_limits<int>::min()) ? x : (x < 0 ? -x : x);
    std::cout << "abs(INT_MIN) handled safely = " << magnitude << "\n";

    // BAD: a range check that runs after the arithmetic is too late, the
    // overflow has already happened.
    // int c = a + b;
    // if (c < a) { /* too late */ }
}

// --- 2. Unsigned arithmetic wraps by definition ---

void unsignedWrapsByDefinition() {
    std::cout << "\n--- Unsigned arithmetic wraps by definition ---\n";

    // Unsigned overflow is defined modulo 2^N, so this is well defined.
    std::uint32_t value = 0xFFFFFFFFu;
    std::uint32_t stepped = value + 1u;
    std::cout << "uint32_t(0xFFFFFFFF) + 1u = " << stepped << "\n";

    std::size_t size_max = std::numeric_limits<std::size_t>::max();
    std::cout << "size_t max + 1 = " << (size_max + 1u) << "\n";
}

// --- 3. Check before you add ---

void checkedAddition() {
    std::cout << "\n--- Check the range before adding ---\n";

    const int max = std::numeric_limits<int>::max();
    const int min = std::numeric_limits<int>::min();

    int out = 0;
    std::cout << "add_checked(1, 2)              = " << add_checked(1, 2, out) << " (" << out << ")\n";
    std::cout << "add_checked(INT_MAX, 1)        = " << add_checked(max, 1, out)
              << " (overflow rejected)\n";
    std::cout << "add_checked(INT_MIN, -1)       = " << add_checked(min, -1, out)
              << " (overflow rejected)\n";

    out = 0;
    std::cout << "sum_to_int(INT_MAX, 1)         = " << sum_to_int(max, 1, out)
              << " (wider intermediate rejected it)\n";
    std::cout << "sum_to_int(1000, 24)           = " << sum_to_int(1000, 24, out) << " (" << out << ")\n";
}

// --- 4. Checked multiplication for sizes ---

void checkedMultiplication() {
    std::cout << "\n--- Checked multiplication for allocation sizes ---\n";

    const std::size_t rows = std::size_t{1} << 40;
    const std::size_t cols = std::size_t{1} << 40;

    std::size_t elements = 0;
    std::cout << "mul_checked(2^40, 2^40) = " << mul_checked(rows, cols, elements)
              << " (overflow rejected)\n";

    // BAD: the unchecked product wraps around to 0 and the vector is created
    // with the wrong size, so it is not executed here.
    // std::vector<int> v(static_cast<std::size_t>(rows * cols));

    std::size_t safe = 0;
    if (mul_checked(1000, 24, safe)) {
        std::vector<int> v(safe);
        std::cout << "mul_checked(1000, 24) = " << safe << " -> vector size: " << v.size() << "\n";
    }
}

int main() {
    signedOverflowIsUndefined();
    unsignedWrapsByDefinition();
    checkedAddition();
    checkedMultiplication();

    std::cout << "\n=== All signed overflow scenarios completed ===\n";
    return 0;
}
