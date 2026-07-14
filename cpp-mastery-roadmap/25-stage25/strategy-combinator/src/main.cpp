#include "strategy_combinator.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Strategy Combinator Demo ===\n\n";

    strategy_combinator::PredicateCombinator combinator(strategy_combinator::CombineMode::All);
    combinator.add(std::make_shared<strategy_combinator::EvenPredicate>());
    combinator.add(std::make_shared<strategy_combinator::PositivePredicate>());

    std::vector<int> values = {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6};
    auto filtered = combinator.filter(values);

    std::cout << "Even and positive: ";
    for (int v : filtered) std::cout << v << " ";
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
