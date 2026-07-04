// examples/virtual_dispatch_example.cpp
// Demonstrates virtual dispatch, polymorphism, and object model in C++.
// Compile with: g++ -std=c++17 -o virtual_dispatch virtual_dispatch_example.cpp

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Base class with virtual functions
class Shape {
public:
    Shape(const std::string& name) : name_(name) {
        std::cout << "[Shape] Created: " << name_ << '\n';
    }
    
    virtual ~Shape() {
        std::cout << "[Shape] Destroyed: " << name_ << '\n';
    }
    
    virtual double area() const = 0;
    virtual void draw() const = 0;
    virtual std::unique_ptr<Shape> clone() const = 0;
    
    std::string name() const { return name_; }

protected:
    std::string name_;
};

// Derived class: Circle
class Circle : public Shape {
public:
    Circle(double radius) : Shape("Circle"), radius_(radius) {
        std::cout << "[Circle] Created with radius: " << radius_ << '\n';
    }
    
    ~Circle() override {
        std::cout << "[Circle] Destroyed\n";
    }
    
    double area() const override {
        return 3.14159265358979 * radius_ * radius_;
    }
    
    void draw() const override {
        std::cout << "Drawing circle with radius " << radius_ << '\n';
    }
    
    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(*this);
    }
    
    double radius() const { return radius_; }

private:
    double radius_;
};

// Derived class: Rectangle
class Rectangle : public Shape {
public:
    Rectangle(double width, double height) 
        : Shape("Rectangle"), width_(width), height_(height) {
        std::cout << "[Rectangle] Created: " << width_ << "x" << height_ << '\n';
    }
    
    ~Rectangle() override {
        std::cout << "[Rectangle] Destroyed\n";
    }
    
    double area() const override {
        return width_ * height_;
    }
    
    void draw() const override {
        std::cout << "Drawing rectangle " << width_ << "x" << height_ << '\n';
    }
    
    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Rectangle>(*this);
    }
    
    double width() const { return width_; }
    double height() const { return height_; }

private:
    double width_;
    double height_;
};

// Function demonstrating polymorphism
void printShapeInfo(const Shape& shape) {
    std::cout << "Shape: " << shape.name() << ", Area: " << shape.area() << '\n';
    shape.draw();
}

// Function demonstrating virtual dispatch
void processShapes(const std::vector<std::unique_ptr<Shape>>& shapes) {
    std::cout << "\n=== Processing Shapes ===\n";
    for (const auto& shape : shapes) {
        printShapeInfo(*shape);
    }
}

// Demonstrate object slicing problem
void demonstrateSlicing() {
    std::cout << "\n=== Object Slicing Demo ===\n";
    
    Circle circle(5.0);
    Shape& shapeRef = circle;  // Reference preserves polymorphism
    
    std::cout << "Via reference - Area: " << shapeRef.area() << '\n';
    
    // Shape shapeCopy = circle;  // This would slice! Uncomment to see compile error
    // std::cout << "Via copy - Area: " << shapeCopy.area() << '\n';  // Would call Shape::area()
}

int main() {
    std::cout << "=== Virtual Dispatch Demo ===\n\n";
    
    // Create shapes using smart pointers for proper cleanup
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(3.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 5.0));
    shapes.push_back(std::make_unique<Circle>(2.5));
    
    processShapes(shapes);
    demonstrateSlicing();
    
    std::cout << "\n=== Cloning Demo ===\n";
    auto original = std::make_unique<Circle>(1.0);
    auto cloned = original->clone();
    
    std::cout << "Original area: " << original->area() << '\n';
    std::cout << "Cloned area: " << cloned->area() << '\n';
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
