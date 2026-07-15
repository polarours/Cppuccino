#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <iostream>

namespace specification_pattern {

class Product {
public:
    Product(std::string name, double price, int stock, bool active)
        : name_(std::move(name)), price_(price), stock_(stock), active_(active) {}

    const std::string& name() const { return name_; }
    double price() const { return price_; }
    int stock() const { return stock_; }
    bool isActive() const { return active_; }

private:
    std::string name_;
    double price_;
    int stock_;
    bool active_;
};

class Specification {
public:
    using CheckFn = std::function<bool(const Product&)>;

    explicit Specification(CheckFn fn) : fn_(std::move(fn)) {}

    bool isSatisfiedBy(const Product& p) const { return fn_(p); }

    Specification andSpec(const Specification& other) const {
        return Specification([self = fn_, other = other.fn_](const Product& p) {
            return self(p) && other(p);
        });
    }

    Specification orSpec(const Specification& other) const {
        return Specification([self = fn_, other = other.fn_](const Product& p) {
            return self(p) || other(p);
        });
    }

    Specification notSpec() const {
        return Specification([self = fn_](const Product& p) {
            return !self(p);
        });
    }

private:
    CheckFn fn_;
};

inline Specification makeActiveSpec() {
    return Specification([](const Product& p) { return p.isActive(); });
}

inline Specification makePriceBelowSpec(double maxPrice) {
    return Specification([maxPrice](const Product& p) { return p.price() < maxPrice; });
}

inline Specification makeInStockSpec() {
    return Specification([](const Product& p) { return p.stock() > 0; });
}

template <typename T>
std::vector<T> filter(const std::vector<T>& items, const Specification& spec) {
    std::vector<T> result;
    for (const auto& item : items) {
        if (spec.isSatisfiedBy(item)) {
            result.push_back(item);
        }
    }
    return result;
}

} // namespace specification_pattern
