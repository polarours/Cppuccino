#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace parameterized_tests {

struct TestCase {
    std::string name;
    std::function<bool()> test;
};

class TestRunner {
public:
    void addTest(const std::string& name, std::function<bool()> test) {
        tests_.push_back({name, std::move(test)});
    }

    struct Results {
        std::size_t total = 0;
        std::size_t passed = 0;
        std::size_t failed = 0;
        std::vector<std::string> failures;
    };

    Results run() const {
        Results results;
        results.total = tests_.size();

        for (const auto& test : tests_) {
            if (test.test()) {
                results.passed++;
            } else {
                results.failed++;
                results.failures.push_back(test.name);
            }
        }

        return results;
    }

    std::size_t testCount() const { return tests_.size(); }

private:
    std::vector<TestCase> tests_;
};

inline bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

inline int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int next = a + b;
        a = b;
        b = next;
    }
    return b;
}

} // namespace parameterized_tests
