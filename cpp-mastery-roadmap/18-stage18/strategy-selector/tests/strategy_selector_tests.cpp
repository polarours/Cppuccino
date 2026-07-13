#include "strategy_selector.hpp"

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

void test_register_strategy() {
    strategy_selector::StrategySelector selector;
    selector.registerStrategy("bubble", std::make_shared<strategy_selector::BubbleStrategy>());
    selector.registerStrategy("quick", std::make_shared<strategy_selector::QuickStrategy>());

    auto strategies = selector.availableStrategies();
    expect(strategies.size() == 2, "expected 2 strategies");
}

void test_select_strategy() {
    strategy_selector::StrategySelector selector;
    selector.registerStrategy("bubble", std::make_shared<strategy_selector::BubbleStrategy>());

    auto strategy = selector.select("bubble");
    expect(strategy != nullptr, "expected strategy");
    expect(strategy->name() == "BubbleSort", "expected BubbleSort");
}

void test_select_nonexistent() {
    strategy_selector::StrategySelector selector;
    auto strategy = selector.select("nonexistent");
    expect(strategy == nullptr, "expected nullptr");
}

void test_use_strategy() {
    strategy_selector::StrategySelector selector;
    selector.registerStrategy("bubble", std::make_shared<strategy_selector::BubbleStrategy>());

    auto strategy = selector.select("bubble");
    std::vector<int> data = {5, 3, 1, 4, 2};
    strategy->sort(data);

    expect(data[0] == 1, "expected 1");
    expect(data[4] == 5, "expected 5");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Selector Tests:\n";
        test_register_strategy();
        std::cout << "  test_register_strategy: PASS\n";

        test_select_strategy();
        std::cout << "  test_select_strategy: PASS\n";

        test_select_nonexistent();
        std::cout << "  test_select_nonexistent: PASS\n";

        test_use_strategy();
        std::cout << "  test_use_strategy: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_selector_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_selector_tests passed" << std::endl;
    return 0;
}
