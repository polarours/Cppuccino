#include "factory_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Factory Pattern Demo ===\n\n";

    auto book = factory_pattern::ProductFactory::create("book", "C++ Primer", 49.99);
    std::cout << book->name() << " - $" << book->price() << "\n";

    auto electronics = factory_pattern::ProductFactory::create("electronics", "Laptop", 999.99);
    std::cout << electronics->name() << " - $" << electronics->price() << "\n";

    auto clothing = factory_pattern::ProductFactory::create("clothing", "T-Shirt", 19.99);
    std::cout << clothing->name() << " - $" << clothing->price() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
