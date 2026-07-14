#include "strategy_chain.hpp"

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

void test_single_strategy() {
    strategy_chain::StrategyChain chain;
    chain.add(std::make_shared<strategy_chain::DoubleStrategy>());

    int result = chain.execute(5);
    expect(result == 10, "expected 10");
    expect(chain.size() == 1, "expected 1 strategy");
}

void test_chained_strategies() {
    strategy_chain::StrategyChain chain;
    chain.add(std::make_shared<strategy_chain::DoubleStrategy>());
    chain.add(std::make_shared<strategy_chain::IncrementStrategy>());

    int result = chain.execute(5);
    expect(result == 11, "expected 11");
}

void test_multiple_chains() {
    strategy_chain::StrategyChain chain;
    chain.add(std::make_shared<strategy_chain::IncrementStrategy>());
    chain.add(std::make_shared<strategy_chain::DoubleStrategy>());
    chain.add(std::make_shared<strategy_chain::IncrementStrategy>());

    int result = chain.execute(5);
    expect(result == 13, "expected 13");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Chain Tests:\n";
        test_single_strategy();
        std::cout << "  test_single_strategy: PASS\n";

        test_chained_strategies();
        std::cout << "  test_chained_strategies: PASS\n";

        test_multiple_chains();
        std::cout << "  test_multiple_chains: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_chain_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_chain_tests passed" << std::endl;
    return 0;
}
