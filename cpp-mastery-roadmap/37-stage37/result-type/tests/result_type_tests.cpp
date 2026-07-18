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
        std::cout << "  test_parse_int_failure: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "result_type_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nresult_type_tests passed" << std::endl;
    return 0;
}
