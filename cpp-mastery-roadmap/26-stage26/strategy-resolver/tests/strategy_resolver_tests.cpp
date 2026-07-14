#include "strategy_resolver.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <cmath>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void expectNear(double a, double b, const std::string& message) {
    if (std::abs(a - b) > 0.01) {
        throw std::runtime_error(message + " (got " + std::to_string(a) + " expected " + std::to_string(b) + ")");
    }
}

void test_resolve_vip() {
    strategy_resolver::StrategyResolver resolver;
    resolver.addResolver([](const std::string& type, int) -> std::shared_ptr<strategy_resolver::PricingStrategy> {
        if (type == "vip") return std::make_shared<strategy_resolver::VIPPricing>();
        return nullptr;
    });

    auto strategy = resolver.resolve("vip", 1);
    expect(strategy->name() == "VIP", "expected VIP");
    expectNear(strategy->calculate(100.0, 1), 80.0, "expected 80");
}

void test_resolve_bulk() {
    strategy_resolver::StrategyResolver resolver;
    resolver.addResolver([](const std::string& type, int qty) -> std::shared_ptr<strategy_resolver::PricingStrategy> {
        if (type == "bulk" && qty >= 50) return std::make_shared<strategy_resolver::BulkPricing>();
        return nullptr;
    });

    auto strategy = resolver.resolve("bulk", 100);
    expect(strategy->name() == "Bulk", "expected Bulk");
    expectNear(strategy->calculate(100.0, 100), 70.0, "expected 70");
}

void test_fallback_to_regular() {
    strategy_resolver::StrategyResolver resolver;

    auto strategy = resolver.resolve("unknown", 1);
    expect(strategy->name() == "Regular", "expected Regular fallback");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Resolver Tests:\n";
        test_resolve_vip();
        std::cout << "  test_resolve_vip: PASS\n";

        test_resolve_bulk();
        std::cout << "  test_resolve_bulk: PASS\n";

        test_fallback_to_regular();
        std::cout << "  test_fallback_to_regular: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_resolver_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_resolver_tests passed" << std::endl;
    return 0;
}
