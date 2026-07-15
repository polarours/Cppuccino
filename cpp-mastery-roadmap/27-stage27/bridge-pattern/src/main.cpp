#include "bridge_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Bridge Pattern Demo ===\n\n";

    auto svg = std::make_shared<bridge_pattern::SVGRenderer>();
    auto canvas = std::make_shared<bridge_pattern::CanvasRenderer>();

    bridge_pattern::Circle c1(svg, 10, 20, 5);
    bridge_pattern::Circle c2(canvas, 10, 20, 5);

    std::cout << "SVG Circle: " << c1.draw() << "\n";
    std::cout << "Canvas Circle: " << c2.draw() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
