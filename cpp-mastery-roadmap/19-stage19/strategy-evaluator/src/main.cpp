#include "strategy_evaluator.hpp"

#include <iostream>
#include <vector>
#include <memory>

int main() {
    std::cout << "=== Strategy Evaluator Demo ===\n\n";

    strategy_evaluator::StrategyEvaluator evaluator;

    std::vector<int> data(100);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = 100 - i;
    }

    evaluator.evaluate(std::make_shared<strategy_evaluator::BubbleSort>(), data);
    evaluator.evaluate(std::make_shared<strategy_evaluator::QuickSort>(), data);

    evaluator.printResults();
    std::cout << "\nBest strategy: " << evaluator.getBestStrategy() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
