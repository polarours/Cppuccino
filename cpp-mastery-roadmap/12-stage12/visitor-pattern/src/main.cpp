#include "visitor_pattern.hpp"

#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::cout << "=== Visitor Pattern Demo ===\n\n";

    std::vector<std::unique_ptr<visitor_pattern::Shape>> shapes;
    shapes.push_back(std::make_unique<visitor_pattern::Circle>(5.0));
    shapes.push_back(std::make_unique<visitor_pattern::Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<visitor_pattern::Triangle>(3.0, 8.0));

    visitor_pattern::AreaCalculator calculator;
    visitor_pattern::Printer printer;

    double totalArea = 0;
    for (auto& shape : shapes) {
        shape->accept(calculator);
        totalArea += calculator.getResult();
        shape->accept(printer);
    }

    std::cout << "\nTotal area: " << totalArea << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
