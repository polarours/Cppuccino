#include "parameterized_tests.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_runner_basic() {
    parameterized_tests::TestRunner runner;
    runner.addTest("pass1", []() { return true; });
    runner.addTest("pass2", []() { return true; });

    auto results = runner.run();
    expect(results.total == 2, "expected 2 tests");
    expect(results.passed == 2, "expected 2 passed");
    expect(results.failed == 0, "expected 0 failed");
}

void test_runner_with_failures() {
    parameterized_tests::TestRunner runner;
    runner.addTest("pass", []() { return true; });
    runner.addTest("fail", []() { return false; });
    runner.addTest("fail2", []() { return false; });

    auto results = runner.run();
    expect(results.total == 3, "expected 3 tests");
    expect(results.passed == 1, "expected 1 passed");
    expect(results.failed == 2, "expected 2 failed");
    expect(results.failures.size() == 2, "expected 2 failure names");
}

void test_parameterized_prime() {
    parameterized_tests::TestRunner runner;

    std::vector<std::pair<int, bool>> cases = {
        {0, false}, {1, false}, {2, true}, {3, true}, {4, false},
        {5, true}, {10, false}, {13, true}, {17, true}, {20, false}
    };

    for (auto [n, expected] : cases) {
        runner.addTest("isPrime(" + std::to_string(n) + ")",
            [n, expected]() { return parameterized_tests::isPrime(n) == expected; });
    }

    auto results = runner.run();
    expect(results.total == 10, "expected 10 tests");
    expect(results.passed == 10, "expected all 10 passed");
}

void test_parameterized_fibonacci() {
    parameterized_tests::TestRunner runner;

    std::vector<std::pair<int, int>> cases = {
        {0, 0}, {1, 1}, {2, 1}, {3, 2}, {4, 3}, {5, 5}, {10, 55}
    };

    for (auto [input, expected] : cases) {
        runner.addTest("fib(" + std::to_string(input) + ")",
            [input, expected]() {
                return parameterized_tests::fibonacci(input) == expected;
            });
    }

    auto results = runner.run();
    expect(results.total == 7, "expected 7 tests");
    expect(results.passed == 7, "expected all 7 passed");
}

} // namespace

int main() {
    try {
        std::cout << "Parameterized Tests:\n";
        test_runner_basic();
        std::cout << "  test_runner_basic: PASS\n";

        test_runner_with_failures();
        std::cout << "  test_runner_with_failures: PASS\n";

        test_parameterized_prime();
        std::cout << "  test_parameterized_prime: PASS\n";

        test_parameterized_fibonacci();
        std::cout << "  test_parameterized_fibonacci: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "parameterized_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nparameterized_tests passed" << std::endl;
    return 0;
}
