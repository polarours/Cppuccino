#include "ranges_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_filter() {
    ranges_pattern::Range range({1, 2, 3, 4, 5, 6});
    auto evens = range.filter([](int x) { return x % 2 == 0; });
    expect(evens.count() == 3, "expected 3 evens");
    expect(evens.data() == std::vector<int>{2, 4, 6}, "expected 2,4,6");
}

void test_map() {
    ranges_pattern::Range range({1, 2, 3});
    auto doubled = range.map([](int x) { return x * 2; });
    expect(doubled.data() == std::vector<int>{2, 4, 6}, "expected 2,4,6");
}

void test_chained() {
    ranges_pattern::Range range({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    auto result = range.filter([](int x) { return x % 2 == 0; })
                      .map([](int x) { return x * 10; })
                      .take(2);
    expect(result.data() == std::vector<int>{20, 40}, "expected 20,40");
}

void test_aggregate() {
    ranges_pattern::Range range({3, 1, 4, 1, 5, 9});
    expect(range.sum() == 23, "expected sum 23");
    expect(range.min() == 1, "expected min 1");
    expect(range.max() == 9, "expected max 9");
}

void test_any_all() {
    ranges_pattern::Range range({2, 4, 6, 8});
    expect(range.all([](int x) { return x % 2 == 0; }), "expected all even");
    expect(!range.any([](int x) { return x % 2 != 0; }), "expected no odd");
}

} // namespace

int main() {
    try {
        std::cout << "Ranges Pattern Tests:\n";
        test_filter();
        std::cout << "  test_filter: PASS\n";

        test_map();
        std::cout << "  test_map: PASS\n";

        test_chained();
        std::cout << "  test_chained: PASS\n";

        test_aggregate();
        std::cout << "  test_aggregate: PASS\n";

        test_any_all();
        std::cout << "  test_any_all: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "ranges_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nranges_pattern_tests passed" << std::endl;
    return 0;
}
