#include "sfinae_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

struct NonPrintable {};

void test_printable_int() {
    auto result = sfinae_pattern::safePrint(42);
    expect(result == "42", "expected 42");
}

void test_printable_string() {
    auto result = sfinae_pattern::safePrint(std::string("hello"));
    expect(result == "hello", "expected hello");
}

void test_non_printable() {
    NonPrintable np;
    auto result = sfinae_pattern::safePrint(np);
    expect(result == "[non-printable]", "expected [non-printable]");
}

void test_has_size_vector() {
    std::vector<int> v = {1, 2, 3};
    expect(sfinae_pattern::getSize(v) == 3, "expected 3");
}

void test_has_size_int() {
    int x = 42;
    expect(sfinae_pattern::getSize(x) == 0, "expected 0 for int");
}

} // namespace

int main() {
    try {
        std::cout << "SFINAE Pattern Tests:\n";
        test_printable_int();
        std::cout << "  test_printable_int: PASS\n";

        test_printable_string();
        std::cout << "  test_printable_string: PASS\n";

        test_non_printable();
        std::cout << "  test_non_printable: PASS\n";

        test_has_size_vector();
        std::cout << "  test_has_size_vector: PASS\n";

        test_has_size_int();
        std::cout << "  test_has_size_int: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "sfinae_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nsfinae_pattern_tests passed" << std::endl;
    return 0;
}
