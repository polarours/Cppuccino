#include "strategy_evaluator.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_evaluate_bubble() {
    strategy_evaluator::StrategyEvaluator evaluator;
    std::vector<int> data = {5, 3, 1, 4, 2};

    evaluator.evaluate(std::make_shared<strategy_evaluator::BubbleSort>(), data);

    auto results = evaluator.getBestStrategy();
    expect(results == "BubbleSort", "expected BubbleSort");
}

void test_evaluate_quick() {
    strategy_evaluator::StrategyEvaluator evaluator;
    std::vector<int> data = {5, 3, 1, 4, 2};

    evaluator.evaluate(std::make_shared<strategy_evaluator::QuickSort>(), data);

    auto results = evaluator.getBestStrategy();
    expect(results == "QuickSort", "expected QuickSort");
}

void test_compare_strategies() {
    strategy_evaluator::StrategyEvaluator evaluator;
    std::vector<int> data(100);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = 100 - i;
    }

    evaluator.evaluate(std::make_shared<strategy_evaluator::BubbleSort>(), data);
    evaluator.evaluate(std::make_shared<strategy_evaluator::QuickSort>(), data);

    auto best = evaluator.getBestStrategy();
    expect(!best.empty(), "expected a best strategy");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Evaluator Tests:\n";
        test_evaluate_bubble();
        std::cout << "  test_evaluate_bubble: PASS\n";

        test_evaluate_quick();
        std::cout << "  test_evaluate_quick: PASS\n";

        test_compare_strategies();
        std::cout << "  test_compare_strategies: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_evaluator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_evaluator_tests passed" << std::endl;
    return 0;
}
