#include "prototype_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Prototype Pattern Demo ===\n\n";

    // Create prototypes
    auto circleProto = std::make_shared<prototype_pattern::Circle>(5.0);
    auto rectProto = std::make_shared<prototype_pattern::Rectangle>(4.0, 6.0);

    // Clone from prototypes
    auto circle1 = circleProto->clone();
    auto circle2 = circleProto->clone();
    auto rect = rectProto->clone();

    std::cout << "Circle 1: " << circle1->type() << " area=" << circle1->area() << "\n";
    std::cout << "Circle 2: " << circle2->type() << " area=" << circle2->area() << "\n";
    std::cout << "Rectangle: " << rect->type() << " area=" << rect->area() << "\n";

    // Using registry
    prototype_pattern::ShapeRegistry registry;
    registry.registerShape("circle", circleProto);
    registry.registerShape("rectangle", rectProto);

    auto fromRegistry = registry.create("circle");
    std::cout << "\nFrom registry: " << fromRegistry->type() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
