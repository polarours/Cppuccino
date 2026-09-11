// examples/bridge-pattern-demo.cpp
// Demonstrates Bridge Pattern: Abstraction with Implementation hierarchy decoupled.
// Compile: g++ -std=c++20 -o bridge-pattern-demo bridge-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace bridge_pattern {

class Drawing {
public:
    virtual ~Drawing() = default;
    virtual void drawLine(int x1, int y1, int x2, int y2) = 0;
    virtual void drawCircle(int x, int y, int radius) = 0;
    virtual void setColor(int r, int g, int b) = 0;
};

class Canvas : public Drawing {
public:
    void drawLine(int x1, int y1, int x2, int y2) override {
        std::cout << "[Canvas] Line (" << x1 << "," << y1 << ") -> (" << x2 << "," << y2 << ")\n";
    }
    void drawCircle(int x, int y, int radius) override {
        std::cout << "[Canvas] Circle (" << x << "," << y << ") r=" << radius << "\n";
    }
    void setColor(int r, int g, int b) override {
        std::cout << "[Canvas] Color (" << r << "," << g << "," << b << ")\n";
    }
};

class SVGExporter : public Drawing {
public:
    void drawLine(int x1, int y1, int x2, int y2) override {
        std::cout << "<line x1=\"" << x1 << "\" y1=\"" << y1
                  << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
    }
    void drawCircle(int x, int y, int radius) override {
        std::cout << "<circle cx=\"" << x << "\" cy=\"" << y
                  << "\" r=\"" << radius << "\"/>\n";
    }
    void setColor(int r, int g, int b) override {
        std::cout << "<g stroke=\"rgb(" << r << "," << g << "," << b << ")\">\n";
    }
};

class Shape {
public:
    explicit Shape(std::shared_ptr<Drawing> drawing)
        : drawing_(std::move(drawing)) {}

    void setRed(int r) { drawing_->setColor(r, 0, 0); }
    void setGreen(int g) { drawing_->setColor(0, g, 0); }
    void setBlue(int b) { drawing_->setColor(0, 0, b); }

protected:
    std::shared_ptr<Drawing> drawing_;
};

class Line : public Shape {
public:
    explicit Line(std::shared_ptr<Drawing> d) : Shape(std::move(d)) {}
    void draw(int x1, int y1, int x2, int y2) {
        drawing_->drawLine(x1, y1, x2, y2);
    }
};

class CircleShape : public Shape {
public:
    explicit CircleShape(std::shared_ptr<Drawing> d) : Shape(std::move(d)) {}
    void draw(int x, int y, int radius) {
        drawing_->drawCircle(x, y, radius);
    }
};

} // namespace bridge_pattern

int main() {
    using namespace bridge_pattern;
    std::cout << "=== Bridge Pattern Demo ===\n\n";

    auto canvas = std::make_shared<Canvas>();
    auto svg = std::make_shared<SVGExporter>();

    std::cout << "--- Canvas Drawing ---\n";
    Line line(canvas);
    line.setRed(255);
    line.draw(0, 0, 100, 100);

    CircleShape circle(canvas);
    circle.setBlue(255);
    circle.draw(50, 50, 30);

    std::cout << "\n--- SVG Export ---\n";
    Line lineSvg(svg);
    lineSvg.setGreen(255);
    lineSvg.draw(0, 0, 200, 200);

    CircleShape circleSvg(svg);
    circleSvg.setRed(255);
    circleSvg.draw(100, 100, 50);

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
