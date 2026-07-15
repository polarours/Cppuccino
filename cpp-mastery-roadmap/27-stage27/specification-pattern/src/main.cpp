#include "specification_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== Specification Pattern Demo ===\n\n";

    std::vector<specification_pattern::Product> products = {
        {"Laptop", 999, 5, true},
        {"Mouse", 25, 10, true},
        {"Old Phone", 50, 0, false},
        {"Monitor", 300, 3, true},
    };

    auto active = specification_pattern::makeActiveSpec();
    auto cheap = specification_pattern::makePriceBelowSpec(100.0);
    auto spec = active.andSpec(cheap);

    auto results = specification_pattern::filter(products, spec);

    std::cout << "Active products under $100:\n";
    for (const auto& p : results) {
        std::cout << "  " << p.name() << " ($" << p.price() << ")\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
