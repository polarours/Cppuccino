#include "strategy_resolver.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Strategy Resolver Demo ===\n\n";

    strategy_resolver::StrategyResolver resolver;
    resolver.addResolver([](const std::string& type, int) -> std::shared_ptr<strategy_resolver::PricingStrategy> {
        if (type == "vip") return std::make_shared<strategy_resolver::VIPPricing>();
        if (type == "bulk") return std::make_shared<strategy_resolver::BulkPricing>();
        return nullptr;
    });

    auto strategy = resolver.resolve("vip", 1);
    std::cout << "Strategy: " << strategy->name() << "\n";
    std::cout << "Price: " << strategy->calculate(100.0, 1) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
