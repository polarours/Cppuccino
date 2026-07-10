#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <map>

namespace prototype_pattern {

// --- Prototype Interface ---

class Shape {
public:
    virtual ~Shape() = default;
    virtual std::unique_ptr<Shape> clone() const = 0;
    virtual std::string type() const = 0;
    virtual double area() const = 0;
};

// --- Concrete Prototypes ---

class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(radius_);
    }

    std::string type() const override { return "Circle"; }
    double area() const override { return 3.14159 * radius_ * radius_; }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Rectangle>(width_, height_);
    }

    std::string type() const override { return "Rectangle"; }
    double area() const override { return width_ * height_; }

private:
    double width_, height_;
};

class Triangle : public Shape {
public:
    Triangle(double base, double height) : base_(base), height_(height) {}

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Triangle>(base_, height_);
    }

    std::string type() const override { return "Triangle"; }
    double area() const override { return 0.5 * base_ * height_; }

private:
    double base_, height_;
};

// --- Prototype Registry ---

class ShapeRegistry {
public:
    void registerShape(const std::string& name, std::shared_ptr<Shape> prototype) {
        prototypes_[name] = prototype;
    }

    std::unique_ptr<Shape> create(const std::string& name) {
        auto it = prototypes_.find(name);
        if (it == prototypes_.end()) {
            throw std::runtime_error("Unknown shape: " + name);
        }
        return it->second->clone();
    }

private:
    std::map<std::string, std::shared_ptr<Shape>> prototypes_;
};

} // namespace prototype_pattern
