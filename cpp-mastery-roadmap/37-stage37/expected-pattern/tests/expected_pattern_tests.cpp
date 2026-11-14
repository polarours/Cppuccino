#include "expected_pattern.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_success() {
    auto result = expected_pattern::divide(10, 2);
    expect(result.hasValue(), "expected hasValue");
    expect(result.value() == 5, "expected 5");
}

void test_error() {
    auto result = expected_pattern::divide(10, 0);
    expect(!result.hasValue(), "expected no value");
    expect(result.error() == "division by zero", "expected division by zero");
}

void test_bool_conversion() {
    auto ok = expected_pattern::divide(10, 2);
    auto err = expected_pattern::divide(10, 0);

    expect(ok ? true : false, "expected truthy");
    expect(!err ? true : false, "expected falsy");
}

void test_map() {
    auto result = expected_pattern::divide(10, 2)
        .map([](int v) { return std::to_string(v) + "x"; });
    expect(result.value() == "5x", "expected 5x");

    auto errResult = expected_pattern::divide(10, 0)
        .map([](int v) { return std::to_string(v) + "x"; });
    expect(!errResult.hasValue(), "expected error propagated");
}

// Regression: the result of the division is not a number an int can
// hold, and the processor traps on it -- the program died with a
// floating point exception.
void test_divide_that_does_not_fit() {
    volatile int a = std::numeric_limits<int>::min();
    volatile int b = -1;

    auto result = expected_pattern::divide(a, b);
    expect(!result.hasValue(), "expected the division to be refused");
    expect(!result.error().empty(), "expected a reason");
}

// A function with an overload for lvalues and one for rvalues. The type
// of the result was worked out from a call with an rvalue, while the
// call itself went with an lvalue -- so the two named different
// overloads and the code did not even compile.
struct Overloaded {
    expected_pattern::Expected<int, std::string> operator()(int&) const {
        return expected_pattern::Expected<int, std::string>::success(1);
    }
    expected_pattern::Expected<long, std::string> operator()(int&&) const {
        return expected_pattern::Expected<long, std::string>::success(2);
    }
};

void test_and_then_with_an_overloaded_function() {
    auto value = expected_pattern::Expected<int, std::string>::success(5);

    auto result = value.andThen(Overloaded{});
    expect(result.hasValue(), "expected the lvalue overload to be used");
    expect(result.value() == 1, "expected 1");
}

void test_read_file() {
    auto ok = expected_pattern::readFile("test.txt");
    expect(ok.hasValue(), "expected success");
    expect(ok.value().find("test.txt") != std::string::npos, "expected filename");

    auto err = expected_pattern::readFile("test.exe");
    expect(!err.hasValue(), "expected error for non-txt");
    expect(err.error().find(".txt") != std::string::npos, "expected error message");
}

} // namespace

int main() {
    try {
        std::cout << "Expected Pattern Tests:\n";
        test_success();
        std::cout << "  test_success: PASS\n";

        test_error();
        std::cout << "  test_error: PASS\n";

        test_bool_conversion();
        std::cout << "  test_bool_conversion: PASS\n";

        test_map();
        std::cout << "  test_map: PASS\n";

        test_read_file();
        std::cout << "  test_read_file: PASS\n";

        test_divide_that_does_not_fit();
        std::cout << "  test_divide_that_does_not_fit: PASS\n";

        test_and_then_with_an_overloaded_function();
        std::cout << "  test_and_then_with_an_overloaded_function: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "expected_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nexpected_pattern_tests passed" << std::endl;
    return 0;
}
