#include "simd_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== SIMD Pattern Demo ===\n\n";

    std::vector<float> a = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<float> b = {8, 7, 6, 5, 4, 3, 2, 1};

    auto sum = simd_pattern::VectorMath::add(a, b);
    std::cout << "add: ";
    for (float v : sum) std::cout << v << " ";
    std::cout << "\n";

    auto prod = simd_pattern::VectorMath::multiply(a, b);
    std::cout << "mul: ";
    for (float v : prod) std::cout << v << " ";
    std::cout << "\n";

    std::cout << "dot: " << simd_pattern::VectorMath::dotProduct(a, b) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
