#include "simd_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void expectNear(float a, float b, const std::string& message) {
    if (std::abs(a - b) > 0.01f) {
        throw std::runtime_error(message + " (got " + std::to_string(a) + ")");
    }
}

void test_vector_add() {
    std::vector<float> a = {1, 2, 3, 4};
    std::vector<float> b = {5, 6, 7, 8};
    auto result = simd_pattern::VectorMath::add(a, b);

    expect(result[0] == 6, "expected 6");
    expect(result[1] == 8, "expected 8");
    expect(result[2] == 10, "expected 10");
    expect(result[3] == 12, "expected 12");
}

void test_vector_multiply() {
    std::vector<float> a = {2, 3, 4, 5};
    std::vector<float> b = {3, 4, 5, 6};
    auto result = simd_pattern::VectorMath::multiply(a, b);

    expect(result[0] == 6, "expected 6");
    expect(result[1] == 12, "expected 12");
    expect(result[2] == 20, "expected 20");
    expect(result[3] == 30, "expected 30");
}

void test_dot_product() {
    std::vector<float> a = {1, 2, 3, 4};
    std::vector<float> b = {5, 6, 7, 8};
    float result = simd_pattern::VectorMath::dotProduct(a, b);

    expectNear(result, 70.0f, "expected 70");
}

void test_scalar_multiply() {
    std::vector<float> a = {1, 2, 3, 4};
    auto result = simd_pattern::VectorMath::scalarMultiply(a, 3.0f);

    expect(result[0] == 3, "expected 3");
    expect(result[1] == 6, "expected 6");
    expect(result[2] == 9, "expected 9");
    expect(result[3] == 12, "expected 12");
}

void test_lerp() {
    std::vector<float> a = {0, 0, 0, 0};
    std::vector<float> b = {10, 10, 10, 10};
    auto result = simd_pattern::VectorMath::lerp(a, b, 0.5f);

    for (int i = 0; i < 4; ++i) {
        expectNear(result[i], 5.0f, "expected 5");
    }
}

void test_unaligned_sizes() {
    std::vector<float> a = {1, 2, 3};
    std::vector<float> b = {4, 5, 6};
    auto result = simd_pattern::VectorMath::add(a, b);

    expect(result.size() == 3, "expected 3");
    expect(result[0] == 5, "expected 5");
    expect(result[1] == 7, "expected 7");
    expect(result[2] == 9, "expected 9");
}

} // namespace

int main() {
    try {
        std::cout << "SIMD Pattern Tests:\n";
        test_vector_add();
        std::cout << "  test_vector_add: PASS\n";

        test_vector_multiply();
        std::cout << "  test_vector_multiply: PASS\n";

        test_dot_product();
        std::cout << "  test_dot_product: PASS\n";

        test_scalar_multiply();
        std::cout << "  test_scalar_multiply: PASS\n";

        test_lerp();
        std::cout << "  test_lerp: PASS\n";

        test_unaligned_sizes();
        std::cout << "  test_unaligned_sizes: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "simd_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nsimd_pattern_tests passed" << std::endl;
    return 0;
}
