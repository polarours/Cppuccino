#include "parameterized_tests.hpp"

#include <iostream>

int main() {
    std::cout << "=== Parameterized Tests Demo ===\n\n";

    parameterized_tests::TestRunner runner;
    std::vector<int> primes = {2, 3, 5, 7, 11, 13};

    for (int p : primes) {
        runner.addTest("prime(" + std::to_string(p) + ")",
            [p]() { return parameterized_tests::isPrime(p); });
    }

    auto results = runner.run();
    std::cout << "Tests: " << results.total << ", Passed: " << results.passed << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
