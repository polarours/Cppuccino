#include "result_type.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_success() {
    auto result = result_type::Result<int>::ok(42);
    expect(result.isSuccess(), "expected success");
    expect(result.value() == 42, "expected 42");
}

void test_failure() {
    auto result = result_type::Result<int>::fail("bad input");
    expect(result.isFailure(), "expected failure");
    expect(result.error().message() == "bad input", "expected error message");
}

void test_value_or() {
    auto success = result_type::Result<int>::ok(10);
    auto failure = result_type::Result<int>::fail("err");

    expect(success.valueOr(0) == 10, "expected 10");
    expect(failure.valueOr(0) == 0, "expected default 0");
}

void test_parse_int_success() {
    auto result = result_type::parseInt("123");
    expect(result.isSuccess(), "expected success");
    expect(result.value() == 123, "expected 123");
}

void test_parse_int_failure() {
    auto result = result_type::parseInt("abc");
    expect(result.isFailure(), "expected failure");
    expect(result.error().message().find("abc") != std::string::npos, "expected error with input");
}

// Regression: std::stoi stops at the first character that is not a
// digit, so "12abc" was read as 12 and the rest of the text was thrown
// away without a word.
void test_parse_int_rejects_trailing_garbage() {
    auto result = result_type::parseInt("123abc");
    expect(result.isFailure(), "expected the trailing garbage to be refused");
    expect(result.error().message().find("123abc") != std::string::npos,
           "expected the error to name the input");

    auto spaced = result_type::parseInt("12 34");
    expect(spaced.isFailure(), "expected the text after the number to be refused");
}

} // namespace

int main() {
    try {
        std::cout << "Result Type Tests:\n";
        test_success();
        std::cout << "  test_success: PASS\n";

        test_failure();
        std::cout << "  test_failure: PASS\n";

        test_value_or();
        std::cout << "  test_value_or: PASS\n";

        test_parse_int_success();
        std::cout << "  test_parse_int_success: PASS\n";

        test_parse_int_failure();
        test_parse_int_rejects_trailing_garbage();
        std::cout << "  test_parse_int_rejects_trailing_garbage: PASS\n";
        std::cout << "  test_parse_int_failure: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "result_type_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nresult_type_tests passed" << std::endl;
    return 0;
}
