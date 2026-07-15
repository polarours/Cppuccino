#pragma once

#include <string>
#include <memory>
#include <iostream>

namespace bridge_pattern {

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual std::string renderCircle(double x, double y, double radius) = 0;
    virtual std::string renderRect(double x, double y, double w, double h) = 0;
};

class SVGRenderer : public Renderer {
public:
    std::string renderCircle(double x, double y, double radius) override {
        return "<circle cx=\"" + std::to_string(x) + "\" cy=\"" + std::to_string(y) +
               "\" r=\"" + std::to_string(radius) + "\"/>";
    }

    std::string renderRect(double x, double y, double w, double h) override {
        return "<rect x=\"" + std::to_string(x) + "\" y=\"" + std::to_string(y) +
               "\" width=\"" + std::to_string(w) + "\" height=\"" + std::to_string(h) + "\"/>";
    }
};

class CanvasRenderer : public Renderer {
public:
    std::string renderCircle(double x, double y, double radius) override {
        return "Canvas.drawCircle(" + std::to_string(x) + ", " + std::to_string(y) +
               ", " + std::to_string(radius) + ")";
    }

    std::string renderRect(double x, double y, double w, double h) override {
        return "Canvas.drawRect(" + std::to_string(x) + ", " + std::to_string(y) +
               ", " + std::to_string(w) + ", " + std::to_string(h) + ")";
    }
};

class Shape {
public:
    explicit Shape(std::shared_ptr<Renderer> renderer) : renderer_(std::move(renderer)) {}
    virtual ~Shape() = default;
    virtual std::string draw() = 0;

protected:
    std::shared_ptr<Renderer> renderer_;
};

class Circle : public Shape {
public:
    Circle(std::shared_ptr<Renderer> renderer, double x, double y, double radius)
        : Shape(std::move(renderer)), x_(x), y_(y), radius_(radius) {}

    std::string draw() override {
        return renderer_->renderCircle(x_, y_, radius_);
    }

private:
    double x_, y_, radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(std::shared_ptr<Renderer> renderer, double x, double y, double w, double h)
        : Shape(std::move(renderer)), x_(x), y_(y), w_(w), h_(h) {}

    std::string draw() override {
        return renderer_->renderRect(x_, y_, w_, h_);
    }

private:
    double x_, y_, w_, h_;
};

} // namespace bridge_pattern
