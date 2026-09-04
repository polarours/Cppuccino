// examples/specification-pattern-demo.cpp
// Demonstrates Specification Pattern with and/or/not composition.
// Compile: g++ -std=c++20 -o specification-pattern-demo specification-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace specification_pattern {

class Product {
public:
    Product(std::string name, double price, int stock, bool active)
        : name_(std::move(name)), price_(price), stock_(stock), active_(active) {}
    const std::string& name()    const { return name_; }
    double              price()  const { return price_; }
    int                 stock()  const { return stock_; }
    bool                isActive() const { return active_; }
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
        return Specification([s=fn_, o=other.fn_](const Product& p){ return s(p) && o(p); });
    }
    Specification orSpec(const Specification& other) const {
        return Specification([s=fn_, o=other.fn_](const Product& p){ return s(p) || o(p); });
    }
    Specification notSpec() const {
        return Specification([s=fn_](const Product& p){ return !s(p); });
    }
private:
    CheckFn fn_;
};

inline Specification activeSpec() {
    return Specification([](const Product& p){ return p.isActive(); });
}
inline Specification priceBelow(double max) {
    return Specification([max](const Product& p){ return p.price() < max; });
}
inline Specification inStockSpec() {
    return Specification([](const Product& p){ return p.stock() > 0; });
}

template<typename T>
std::vector<T> filterItems(const std::vector<T>& items, const Specification& spec) {
    std::vector<T> r;
    for (const auto& item : items)
        if (spec.isSatisfiedBy(item)) r.push_back(item);
    return r;
}

} // namespace specification_pattern

int main() {
    using namespace specification_pattern;
    std::cout << "=== Specification Pattern Demo ===\n\n";

    std::vector<Product> products{
        {"Laptop",  999.0, 5,  true},
        {"Keyboard", 79.0, 0,  true},
        {"Mouse",    29.0, 20, false},
        {"Monitor", 299.0, 3,  true},
    };

    auto spec = activeSpec().andSpec(priceBelow(500.0)).andSpec(inStockSpec());
    auto results = filterItems(products, spec);

    std::cout << "Active + price < $500 + in stock:\n";
    for (const auto& p : results)
        std::cout << "  " << p.name() << " ($" << p.price() << ")\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
