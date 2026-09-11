// examples/visitor-pattern-demo.cpp
// Demonstrates Visitor Pattern: double dispatch for Shapes.
// Compile: g++ -std=c++20 -o visitor-pattern-demo visitor-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace visitor_pattern {

class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visitCircle(const class Circle&) = 0;
    virtual void visitRectangle(const class Rectangle&) = 0;
    virtual void visitTriangle(const class Triangle&) = 0;
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor& v) = 0;
    virtual std::string name() const = 0;
};

class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}
    void accept(ShapeVisitor& v) override { v.visitCircle(*this); }
    std::string name() const override { return "Circle"; }
    double radius() const { return radius_; }
private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}
    void accept(ShapeVisitor& v) override { v.visitRectangle(*this); }
    std::string name() const override { return "Rectangle"; }
    double width()  const { return width_; }
    double height() const { return height_; }
private:
    double width_, height_;
};

class Triangle : public Shape {
public:
    Triangle(double base, double height) : base_(base), height_(height) {}
    void accept(ShapeVisitor& v) override { v.visitTriangle(*this); }
    std::string name() const override { return "Triangle"; }
    double base()   const { return base_; }
    double height() const { return height_; }
private:
    double base_, height_;
};

class AreaCalculator : public ShapeVisitor {
public:
    void visitCircle(const Circle& c) override    { area_ += 3.14159265 * c.radius() * c.radius(); }
    void visitRectangle(const Rectangle& r) override { area_ += r.width() * r.height(); }
    void visitTriangle(const Triangle& t) override    { area_ += 0.5 * t.base() * t.height(); }
    double getResult() const { return area_; }
private:
    double area_ = 0.0;
};

class Printer : public ShapeVisitor {
public:
    void visitCircle(const Circle& c) override    { std::cout << "  Circle: r=" << c.radius() << "\n"; }
    void visitRectangle(const Rectangle& r) override { std::cout << "  Rectangle: " << r.width() << "x" << r.height() << "\n"; }
    void visitTriangle(const Triangle& t) override    { std::cout << "  Triangle: base=" << t.base() << ", height=" << t.height() << "\n"; }
};

} // namespace visitor_pattern

int main() {
    using namespace visitor_pattern;
    std::cout << "=== Visitor Pattern Demo ===\n\n";

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 8.0));

    AreaCalculator calculator;
    Printer printer;

    std::cout << "Calculating areas:\n";
    double totalArea = 0;
    for (auto& shape : shapes) {
        shape->accept(calculator);
        totalArea += calculator.getResult() - (totalArea > 0 ? totalArea : 0);
        shape->accept(printer);
    }
    std::cout << "Total area: " << calculator.getResult() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
