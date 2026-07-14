#include "strategy_factory.hpp"

#include <iostream>

int main() {
    std::cout << "=== Strategy Factory Demo ===\n\n";

    strategy_factory::StrategyFactory factory;
    factory.registerStrategy("test", []() -> std::shared_ptr<strategy_factory::SortStrategy> {
        return nullptr;
    });

    std::cout << "Has 'test': " << std::boolalpha << factory.hasStrategy("test") << "\n";
    std::cout << "Has 'other': " << factory.hasStrategy("other") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
