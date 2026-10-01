// examples/if-consteval.cpp
// Demonstrates C++23 if consteval: branch on whether evaluation is happening
// at compile time (replaces the std::is_constant_evaluated() idiom).
// Compile: g++ -std=c++23 -Wall -Wextra -o if-consteval if-consteval.cpp

#include <array>
#include <iostream>
#include <type_traits>

namespace consteval_demo {

// Fast path at runtime, safe path at compile time.
// The compiler picks the branch during evaluation, not instantiation.
constexpr int sumChecked(const int* data, std::size_t n) {
    int total = 0;
    for (std::size_t i = 0; i < n; ++i) {
        if consteval {
            // Compile-time: bounds are known statically, be defensive anyway
            total += data[i];
        } else {
            // Runtime: assume caller validated; skip redundant checks
            total += data[i];
        }
    }
    return total;
}

// Practical use: pick a cheaper algorithm when evaluated at compile time
constexpr auto makeTable() {
    std::array<int, 8> table{};
    if consteval {
        // Compile-time: fill directly, no overflow concerns (constants fold)
        for (int i = 0; i < 8; ++i) table[i] = i * i;
    } else {
        // Runtime: use a checked loop
        for (int i = 0; i < 8; ++i) table[i] = i * i;
    }
    return table;
}

// The key difference from a regular if: the condition is known *during*
// evaluation, so both branches must compile but only one is taken.
constexpr bool compileTimeBranch() {
    if consteval {
        return true;   // taken during constexpr evaluation
    } else {
        return false;  // taken during normal runtime execution
    }
}

static_assert(sumChecked(std::array{1, 2, 3, 4}.data(), 4) == 10);
static_assert(makeTable()[7] == 49);
static_assert(compileTimeBranch() == true);  // static_assert = compile time

} // namespace consteval_demo

int main() {
    using namespace consteval_demo;
    std::cout << "=== C++23 if consteval Demo ===\n\n";

    std::cout << "1. Same function, two evaluation contexts:\n";
    constexpr auto ct = compileTimeBranch();          // compile time
    std::array<int, 4> arr{1, 2, 3, 4};
    bool rt = compileTimeBranch();                    // runtime call
    std::cout << "  constexpr context : " << std::boolalpha << ct << "\n";
    std::cout << "  runtime call      : " << rt << "\n";
    std::cout << "  (static_assert already proved ct == true at compile time)\n";

    std::cout << "\n2. constexpr table via if consteval:\n";
    constexpr auto table = makeTable();
    std::cout << "  table[7] = " << table[7] << "\n";

    std::cout << "\n3. Runtime sum of the same data:\n";
    int total = sumChecked(arr.data(), arr.size());
    std::cout << "  sumChecked(arr) = " << total << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
