#include "strategy_selector.hpp"

#include <iostream>
#include <vector>

int main() {
    std::cout << "=== Strategy Selector Demo ===\n\n";

    strategy_selector::StrategySelector selector;
    selector.registerStrategy("bubble", std::make_shared<strategy_selector::BubbleStrategy>());
    selector.registerStrategy("quick", std::make_shared<strategy_selector::QuickStrategy>());

    std::vector<int> data = {5, 3, 1, 4, 2};

    auto strategy = selector.select("bubble");
    std::cout << "Using: " << strategy->name() << "\n";
    strategy->sort(data);

    std::cout << "Sorted: ";
    for (int n : data) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
