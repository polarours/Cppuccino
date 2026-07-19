#include "zip_range.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_zip() {
    std::vector<int> a = {1, 2, 3};
    std::vector<std::string> b = {"a", "b", "c"};

    int sum = 0;
    for (auto [x, y] : zip_range::zip(a, b)) {
        sum += x;
    }
    expect(sum == 6, "expected sum 6");
}

void test_zip_different_sizes() {
    std::vector<int> a = {1, 2, 3, 4, 5};
    std::vector<int> b = {10, 20};

    int sum = 0;
    for (auto [x, y] : zip_range::zip(a, b)) {
        sum += x + y;
    }
    expect(sum == 33, "expected 33 (1+10 + 2+20)");
}

void test_zip_three_vectors() {
    std::vector<int> a = {1, 2};
    std::vector<int> b = {10, 20};
    std::vector<int> c = {100, 200};

    int sum = 0;
    for (auto [x, y, z] : zip_range::zip(a, b, c)) {
        sum += x + y + z;
    }
    expect(sum == 333, "expected 333");
}

void test_zip_size() {
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = {1, 2, 3, 4, 5};

    auto range = zip_range::zip(a, b);
    expect(range.size() == 3, "expected min size 3");
}

} // namespace

int main() {
    try {
        std::cout << "Zip Range Tests:\n";
        test_basic_zip();
        std::cout << "  test_basic_zip: PASS\n";

        test_zip_different_sizes();
        std::cout << "  test_zip_different_sizes: PASS\n";

        test_zip_three_vectors();
        std::cout << "  test_zip_three_vectors: PASS\n";

        test_zip_size();
        std::cout << "  test_zip_size: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "zip_range_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nzip_range_tests passed" << std::endl;
    return 0;
}
