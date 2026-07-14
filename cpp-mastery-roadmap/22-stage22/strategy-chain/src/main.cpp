#include "strategy_chain.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Strategy Chain Demo ===\n\n";

    strategy_chain::StrategyChain chain;
    chain.add(std::make_shared<strategy_chain::DoubleStrategy>());
    chain.add(std::make_shared<strategy_chain::IncrementStrategy>());

    int result = chain.execute(5);
    std::cout << "Result: " << result << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
