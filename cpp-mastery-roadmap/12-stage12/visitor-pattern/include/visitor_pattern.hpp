#pragma once

#include <string>
#include <iostream>
#include <memory>

namespace visitor_pattern {

// --- Visitor Interface ---

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(class Circle& circle) = 0;
    virtual void visit(class Rectangle& rectangle) = 0;
    virtual void visit(class Triangle& triangle) = 0;
};

// --- Element Interface ---

class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string type() const = 0;
};

// --- Concrete Elements ---

class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}

    void accept(Visitor& visitor) override { visitor.visit(*this); }
    std::string type() const override { return "Circle"; }
    double getRadius() const { return radius_; }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    void accept(Visitor& visitor) override { visitor.visit(*this); }
    std::string type() const override { return "Rectangle"; }
    double getWidth() const { return width_; }
    double getHeight() const { return height_; }

private:
    double width_, height_;
};

class Triangle : public Shape {
public:
    Triangle(double base, double height) : base_(base), height_(height) {}

    void accept(Visitor& visitor) override { visitor.visit(*this); }
    std::string type() const override { return "Triangle"; }
    double getBase() const { return base_; }
    double getHeight() const { return height_; }

private:
    double base_, height_;
};

// --- Concrete Visitors ---

class AreaCalculator : public Visitor {
public:
    void visit(Circle& circle) override {
        result_ = 3.14159 * circle.getRadius() * circle.getRadius();
    }

    void visit(Rectangle& rectangle) override {
        result_ = rectangle.getWidth() * rectangle.getHeight();
    }

    void visit(Triangle& triangle) override {
        result_ = 0.5 * triangle.getBase() * triangle.getHeight();
    }

    double getResult() const { return result_; }

private:
    double result_ = 0;
};

class Printer : public Visitor {
public:
    void visit(Circle& circle) override {
        std::cout << "Circle with radius " << circle.getRadius() << "\n";
    }

    void visit(Rectangle& rectangle) override {
        std::cout << "Rectangle " << rectangle.getWidth() << "x" << rectangle.getHeight() << "\n";
    }

    void visit(Triangle& triangle) override {
        std::cout << "Triangle base=" << triangle.getBase() << " height=" << triangle.getHeight() << "\n";
    }
};

} // namespace visitor_pattern
