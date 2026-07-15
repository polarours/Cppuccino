#include "bridge_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_svg_circle() {
    auto renderer = std::make_shared<bridge_pattern::SVGRenderer>();
    bridge_pattern::Circle circle(renderer, 10, 20, 5);

    auto result = circle.draw();
    expect(result.find("<circle") != std::string::npos, "expected SVG circle");
    expect(result.find("cx=") != std::string::npos, "expected cx attribute");
}

void test_canvas_rect() {
    auto renderer = std::make_shared<bridge_pattern::CanvasRenderer>();
    bridge_pattern::Rectangle rect(renderer, 0, 0, 100, 50);

    auto result = rect.draw();
    expect(result.find("Canvas.drawRect") != std::string::npos, "expected Canvas rect");
}

void test_bridge_separation() {
    auto svg = std::make_shared<bridge_pattern::SVGRenderer>();
    auto canvas = std::make_shared<bridge_pattern::CanvasRenderer>();

    bridge_pattern::Circle c1(svg, 0, 0, 10);
    bridge_pattern::Circle c2(canvas, 0, 0, 10);

    expect(c1.draw().find("<circle") != std::string::npos, "expected SVG");
    expect(c2.draw().find("Canvas") != std::string::npos, "expected Canvas");
}

} // namespace

int main() {
    try {
        std::cout << "Bridge Pattern Tests:\n";
        test_svg_circle();
        std::cout << "  test_svg_circle: PASS\n";

        test_canvas_rect();
        std::cout << "  test_canvas_rect: PASS\n";

        test_bridge_separation();
        std::cout << "  test_bridge_separation: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "bridge_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nbridge_pattern_tests passed" << std::endl;
    return 0;
}
