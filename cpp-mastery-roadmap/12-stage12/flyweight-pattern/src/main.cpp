#include "flyweight_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Flyweight Pattern Demo ===\n\n";

    flyweight_pattern::FontFactory factory;

    // Create shared fonts
    auto arial12 = factory.getFont("Arial", 12, false);
    auto arial14 = factory.getFont("Arial", 14, false);
    auto bold12 = factory.getFont("Arial", 12, true);

    std::cout << "Unique fonts: " << factory.size() << "\n\n";

    // Create text characters with shared fonts
    std::string text = "Hello";
    for (char ch : text) {
        flyweight_pattern::TextCharacter tc(ch, arial12);
        tc.render();
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
