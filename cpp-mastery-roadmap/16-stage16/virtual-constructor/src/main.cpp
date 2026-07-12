#include "virtual_constructor.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Virtual Constructor Demo ===\n\n";

    // Create shapes via factory
    auto circle = virtual_constructor::ShapeFactory::create(
        virtual_constructor::ShapeFactory::ShapeType::Circle, 5.0);
    auto rect = virtual_constructor::ShapeFactory::create(
        virtual_constructor::ShapeFactory::ShapeType::Rectangle, 4.0, 6.0);

    // Clone shapes
    auto circleCopy = circle->clone();
    auto rectCopy = rect->clone();

    std::cout << "Original Circle: ";
    circle->print();
    std::cout << "Cloned Circle: ";
    circleCopy->print();

    std::cout << "Original Rectangle: ";
    rect->print();
    std::cout << "Cloned Rectangle: ";
    rectCopy->print();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
