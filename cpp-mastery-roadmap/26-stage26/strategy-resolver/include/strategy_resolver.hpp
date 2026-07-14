#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

namespace strategy_resolver {

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double calculate(double basePrice, int quantity) const = 0;
    virtual std::string name() const = 0;
};

class RegularPricing : public PricingStrategy {
public:
    double calculate(double basePrice, int) const override {
        return basePrice;
    }
    std::string name() const override { return "Regular"; }
};

class BulkPricing : public PricingStrategy {
public:
    double calculate(double basePrice, int quantity) const override {
        if (quantity >= 100) return basePrice * 0.7;
        if (quantity >= 50) return basePrice * 0.85;
        return basePrice * 0.95;
    }
    std::string name() const override { return "Bulk"; }
};

class VIPPricing : public PricingStrategy {
public:
    double calculate(double basePrice, int) const override {
        return basePrice * 0.8;
    }
    std::string name() const override { return "VIP"; }
};

using Resolver = std::function<std::shared_ptr<PricingStrategy>(const std::string& customerType, int quantity)>;

class StrategyResolver {
public:
    void addResolver(Resolver resolver) {
        resolvers_.push_back(std::move(resolver));
    }

    std::shared_ptr<PricingStrategy> resolve(const std::string& customerType, int quantity) const {
        for (const auto& resolver : resolvers_) {
            auto strategy = resolver(customerType, quantity);
            if (strategy) return strategy;
        }
        return std::make_shared<RegularPricing>();
    }

private:
    std::vector<Resolver> resolvers_;
};

} // namespace strategy_resolver
