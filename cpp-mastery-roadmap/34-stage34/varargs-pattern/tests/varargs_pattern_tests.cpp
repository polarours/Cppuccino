#include "varargs_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_format_single() {
    auto result = varargs_pattern::format("Hello, {}!", std::string("World"));
    expect(result == "Hello, World!", "expected Hello, World!");
}

void test_format_multiple() {
    auto result = varargs_pattern::format("{} is {} years old", std::string("Alice"), std::to_string(30));
    expect(result == "Alice is 30 years old", "expected formatted string");
}

void test_format_no_args() {
    auto result = varargs_pattern::format("No placeholders here");
    expect(result == "No placeholders here", "expected no change");
}

void test_make_vector() {
    auto vec = varargs_pattern::makeVector(1, 2, 3, 4, 5);
    expect(vec.size() == 5, "expected 5 elements");
    expect(vec[0] == "1", "expected 1");
    expect(vec[4] == "5", "expected 5");
}

void test_format_empty() {
    auto result = varargs_pattern::format("");
    expect(result.empty(), "expected empty string");
}

} // namespace

int main() {
    try {
        std::cout << "Varargs Pattern Tests:\n";
        test_format_single();
        std::cout << "  test_format_single: PASS\n";

        test_format_multiple();
        std::cout << "  test_format_multiple: PASS\n";

        test_format_no_args();
        std::cout << "  test_format_no_args: PASS\n";

        test_make_vector();
        std::cout << "  test_make_vector: PASS\n";

        test_format_empty();
        std::cout << "  test_format_empty: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "varargs_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nvarargs_pattern_tests passed" << std::endl;
    return 0;
}
