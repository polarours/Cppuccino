#pragma once

#include <string>
#include <memory>
#include <iostream>

namespace virtual_constructor {

// --- Base Class ---

class Shape {
public:
    virtual ~Shape() = default;

    // Virtual constructor (clone)
    virtual std::unique_ptr<Shape> clone() const = 0;

    virtual double area() const = 0;
    virtual std::string type() const = 0;

    virtual void print() const {
        std::cout << type() << " with area " << area() << "\n";
    }
};

// --- Concrete Shapes ---

class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(radius_);
    }

    double area() const override { return 3.14159 * radius_ * radius_; }
    std::string type() const override { return "Circle"; }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Rectangle>(width_, height_);
    }

    double area() const override { return width_ * height_; }
    std::string type() const override { return "Rectangle"; }

private:
    double width_, height_;
};

class Triangle : public Shape {
public:
    Triangle(double base, double height) : base_(base), height_(height) {}

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Triangle>(base_, height_);
    }

    double area() const override { return 0.5 * base_ * height_; }
    std::string type() const override { return "Triangle"; }

private:
    double base_, height_;
};

// --- Shape Factory ---

class ShapeFactory {
public:
    enum class ShapeType { Circle, Rectangle, Triangle };

    static std::unique_ptr<Shape> create(ShapeType type, double param1, double param2 = 0) {
        switch (type) {
            case ShapeType::Circle: return std::make_unique<Circle>(param1);
            case ShapeType::Rectangle: return std::make_unique<Rectangle>(param1, param2);
            case ShapeType::Triangle: return std::make_unique<Triangle>(param1, param2);
        }
        return nullptr;
    }
};

} // namespace virtual_constructor
