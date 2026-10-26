// examples/simd-vectorization.cpp
// Demonstrates a hand written vectorized loop and the scalar tail beside it.
// Compile with: g++ -std=c++17 -O2 -o simd_vectorization simd-vectorization.cpp

#include <chrono>
#include <cstddef>
#include <iostream>
#include <vector>

// The scalar version: one element per iteration.
std::vector<float> addScalar(const std::vector<float>& a,
                             const std::vector<float>& b) {
    std::vector<float> result(a.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

// The vectorized version: four elements per iteration, and a scalar loop
// for the remainder. The bound "i + 4 <= size" is what keeps the wide
// loop from running past the end of the vector.
std::vector<float> addVectorized(const std::vector<float>& a,
                                 const std::vector<float>& b) {
    std::vector<float> result(a.size());

    std::size_t i = 0;
    for (; i + 4 <= a.size(); i += 4) {
        result[i] = a[i] + b[i];
        result[i + 1] = a[i + 1] + b[i + 1];
        result[i + 2] = a[i + 2] + b[i + 2];
        result[i + 3] = a[i + 3] + b[i + 3];
    }
    // The tail: the last one to three elements.
    for (; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

// The same work, but the compiler is left to vectorize it. The wide loop
// is written so that the compiler has no reason to doubt the alignment or
// the bounds.
std::vector<float> addAutoVectorized(const std::vector<float>& a,
                                     const std::vector<float>& b) {
    std::vector<float> result(a.size());
    const float* pa = a.data();
    const float* pb = b.data();
    float* pr = result.data();
    const std::size_t count = a.size();
    for (std::size_t i = 0; i < count; ++i) {
        pr[i] = pa[i] + pb[i];
    }
    return result;
}

bool same(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

template <typename Function>
double timeMs(Function function) {
    auto start = std::chrono::steady_clock::now();
    function();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

int main() {
    std::cout << "=== Vectorization ===\n\n";

    // A size that is not a multiple of four, so the tail has work to do.
    constexpr std::size_t count = 1'000'003;
    std::vector<float> a(count);
    std::vector<float> b(count);
    for (std::size_t i = 0; i < count; ++i) {
        a[i] = static_cast<float>(i % 100);
        b[i] = static_cast<float>(i % 7);
    }

    std::cout << "--- 1. The vectorized loop and its tail ---\n";
    std::cout << "  " << count << " elements, " << count % 4
              << " of them in the scalar tail\n";

    const auto scalar = addScalar(a, b);
    const auto vectorized = addVectorized(a, b);
    const auto automatic = addAutoVectorized(a, b);
    std::cout << "  the vectorized loop agrees with the scalar one: "
              << (same(scalar, vectorized) ? "yes" : "no") << "\n";
    std::cout << "  the compiler's version agrees as well: "
              << (same(scalar, automatic) ? "yes" : "no") << "\n";

    std::cout << "\n--- 2. How long each one takes ---\n";
    constexpr int rounds = 20;

    auto scalarTime = timeMs([&] {
        for (int i = 0; i < rounds; ++i) {
            volatile auto sink = addScalar(a, b);
            (void)sink;
        }
    });
    auto vectorTime = timeMs([&] {
        for (int i = 0; i < rounds; ++i) {
            volatile auto sink = addVectorized(a, b);
            (void)sink;
        }
    });
    auto autoTime = timeMs([&] {
        for (int i = 0; i < rounds; ++i) {
            volatile auto sink = addAutoVectorized(a, b);
            (void)sink;
        }
    });

    std::cout << "  scalar:          " << scalarTime << " ms for " << rounds
              << " rounds\n";
    std::cout << "  hand vectorized: " << vectorTime << " ms for " << rounds
              << " rounds\n";
    std::cout << "  compiler:        " << autoTime << " ms for " << rounds
              << " rounds\n";
    std::cout << "  the numbers depend on the machine, the compiler and the\n"
                 "  flags. What does not depend on them is that the wide\n"
                 "  loop has to be followed by a tail, or the last few\n"
                 "  elements are never added.\n";

    std::cout << "\n--- 3. The tail is the part that is easy to forget ---\n";
    std::vector<float> small(3, 1.0f);
    const auto smallResult = addVectorized(small, small);
    std::cout << "  three elements: " << smallResult[0] << " " << smallResult[1]
              << " " << smallResult[2] << "\n";
    std::cout << "  without the tail they would have stayed at zero\n";

    std::cout << "\n=== All vectorization scenarios completed ===\n";
    return 0;
}
