#include "strategy_decorator.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Strategy Decorator Demo ===\n\n";

    auto strategy = std::make_shared<strategy_decorator::LoggingDecorator>(
        std::make_shared<strategy_decorator::DoubleStrategy>());

    std::cout << "Result: " << strategy->execute(5) << "\n";
    std::cout << "Name: " << strategy->name() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
