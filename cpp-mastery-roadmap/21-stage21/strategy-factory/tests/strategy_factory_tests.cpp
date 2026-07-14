#include "strategy_factory.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_register_and_create() {
    strategy_factory::StrategyFactory factory;
    factory.registerStrategy("bubble", []() {
        return std::make_shared<strategy_factory::BubbleSortStrategy>();
    });

    expect(factory.hasStrategy("bubble"), "expected bubble to exist");
    expect(!factory.hasStrategy("other"), "expected other to not exist");
}

void test_create_strategy() {
    strategy_factory::StrategyFactory factory;
    factory.registerStrategy("bubble", []() {
        return std::make_shared<strategy_factory::BubbleSortStrategy>();
    });

    auto strategy = factory.create("bubble");
    expect(strategy != nullptr, "expected strategy");

    std::vector<int> data = {5, 3, 1, 4, 2};
    strategy->sort(data);
    expect(data[0] == 1, "expected 1");
    expect(data[4] == 5, "expected 5");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Factory Tests:\n";
        test_register_and_create();
        std::cout << "  test_register_and_create: PASS\n";

        test_create_strategy();
        std::cout << "  test_create_strategy: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_factory_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_factory_tests passed" << std::endl;
    return 0;
}
