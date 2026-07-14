#include "strategy_decorator.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_base_strategy() {
    auto strategy = std::make_shared<strategy_decorator::DoubleStrategy>();
    expect(strategy->execute(5) == 10, "expected 10");
    expect(strategy->name() == "Double", "expected Double");
}

void test_logging_decorator() {
    auto strategy = std::make_shared<strategy_decorator::LoggingDecorator>(
        std::make_shared<strategy_decorator::IncrementStrategy>());

    expect(strategy->execute(5) == 6, "expected 6");
    expect(strategy->name() == "Logging(Increment)", "expected name");
}

void test_timing_decorator() {
    auto strategy = std::make_shared<strategy_decorator::TimingDecorator>(
        std::make_shared<strategy_decorator::DoubleStrategy>());

    int result = strategy->execute(5);
    expect(result == 10, "expected 10");
    expect(strategy->lastDurationUs() >= 0, "expected non-negative duration");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Decorator Tests:\n";
        test_base_strategy();
        std::cout << "  test_base_strategy: PASS\n";

        test_logging_decorator();
        std::cout << "  test_logging_decorator: PASS\n";

        test_timing_decorator();
        std::cout << "  test_timing_decorator: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_decorator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_decorator_tests passed" << std::endl;
    return 0;
}
