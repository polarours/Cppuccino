#include "concepts_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_sum() {
    std::vector<int> ints = {1, 2, 3, 4, 5};
    expect(concepts_pattern::sum(ints) == 15, "expected 15");

    std::vector<double> doubles = {1.5, 2.5, 3.0};
    expect(concepts_pattern::sum(doubles) == 7.0, "expected 7.0");
}

void test_average() {
    std::vector<int> values = {2, 4, 6, 8};
    expect(concepts_pattern::average(values) == 5, "expected 5");

    std::vector<int> empty;
    expect(concepts_pattern::average(empty) == 0, "expected 0 for empty");
}

void test_describe() {
    concepts_pattern::Point p(3.0, 4.0);
    auto result = concepts_pattern::describe(p);
    expect(result.find("3.0") != std::string::npos, "expected 3.0 in description");
    expect(result.find("4.0") != std::string::npos, "expected 4.0 in description");
}

void test_print_container() {
    std::vector<int> v = {10, 20, 30};
    concepts_pattern::printContainer(v);
}

void test_concepts_compile() {
    static_assert(concepts_pattern::Numeric<int>, "int is Numeric");
    static_assert(concepts_pattern::Numeric<double>, "double is Numeric");
    static_assert(!concepts_pattern::Numeric<std::string>, "string is not Numeric");
}

} // namespace

int main() {
    try {
        std::cout << "Concepts Pattern Tests:\n";
        test_sum();
        std::cout << "  test_sum: PASS\n";

        test_average();
        std::cout << "  test_average: PASS\n";

        test_describe();
        std::cout << "  test_describe: PASS\n";

        test_print_container();
        std::cout << "  test_print_container: PASS\n";

        test_concepts_compile();
        std::cout << "  test_concepts_compile: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "concepts_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nconcepts_pattern_tests passed" << std::endl;
    return 0;
}
