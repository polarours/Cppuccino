// examples/virtual_dispatch_and_slicing.cpp
// Demonstrates virtual dispatch, object slicing, and polymorphism.
// Compile with: g++ -std=c++17 -o virtual_dispatch virtual-dispatch-and-slicing.cpp

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// --- 1. Basic Virtual Dispatch ---

class Shape {
public:
    virtual ~Shape() = default;

    virtual double area() const { return 0.0; }
    virtual std::string name() const { return "Shape"; }

    void describe() const {
        std::cout << name() << " with area = " << area() << "\n";
    }
};

class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}

    double area() const override {
        return 3.14159 * radius_ * radius_;
    }

    std::string name() const override {
        return "Circle";
    }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    double area() const override {
        return width_ * height_;
    }

    std::string name() const override {
        return "Rectangle";
    }

private:
    double width_, height_;
};

class Triangle : public Shape {
public:
    Triangle(double base, double height) : base_(base), height_(height) {}

    double area() const override {
        return 0.5 * base_ * height_;
    }

    std::string name() const override {
        return "Triangle";
    }

private:
    double base_, height_;
};

void virtualDispatchDemo() {
    std::cout << "--- Virtual Dispatch ---\n";

    Circle c(5.0);
    Rectangle r(4.0, 6.0);
    Triangle t(3.0, 8.0);

    c.describe();
    r.describe();
    t.describe();
}

// --- 2. Object Slicing Problem ---

void objectSlicingProblem() {
    std::cout << "\n--- Object Slicing Problem ---\n";

    Circle c(5.0);

    // BAD: Slicing occurs when assigning to base type by value
    Shape sliced = c;  // Circle part is sliced off!
    std::cout << "Sliced shape: " << sliced.name() << " area = " << sliced.area() << "\n";

    // This compiles but the Circle-specific data is lost
    // The vtable pointer still points to Shape, not Circle
}

// --- 3. Avoiding Slicing with Pointers and References ---

void noSlicingDemo() {
    std::cout << "\n--- No Slicing (Pointer/Reference) ---\n";

    Circle c(5.0);

    // GOOD: Reference preserves polymorphic behavior
    Shape& ref = c;
    std::cout << "Reference: " << ref.name() << " area = " << ref.area() << "\n";

    // GOOD: Pointer preserves polymorphic behavior
    Shape* ptr = &c;
    std::cout << "Pointer: " << ptr->name() << " area = " << ptr->area() << "\n";
}

// --- 4. Polymorphism with Smart Pointers ---

void polymorphismWithSmartPointers() {
    std::cout << "\n--- Polymorphism with Smart Pointers ---\n";

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(3.0));
    shapes.push_back(std::make_unique<Rectangle>(2.0, 7.0));
    shapes.push_back(std::make_unique<Triangle>(4.0, 5.0));

    double totalArea = 0.0;
    for (const auto& shape : shapes) {
        shape->describe();
        totalArea += shape->area();
    }

    std::cout << "Total area: " << totalArea << "\n";
}

// --- 5. Slicing in Containers ---

void slicingInContainers() {
    std::cout << "\n--- Slicing in Containers ---\n";

    // BAD: std::vector<Shape> causes slicing
    std::vector<Shape> badShapes;
    badShapes.push_back(Circle(5.0));     // Sliced!
    badShapes.push_back(Rectangle(3.0, 4.0));  // Sliced!

    std::cout << "Vector of Shape (sliced):\n";
    for (const auto& s : badShapes) {
        std::cout << "  " << s.name() << " area = " << s.area() << "\n";
    }

    // GOOD: std::vector<std::unique_ptr<Shape>> preserves polymorphism
    std::vector<std::unique_ptr<Shape>> goodShapes;
    goodShapes.push_back(std::make_unique<Circle>(5.0));
    goodShapes.push_back(std::make_unique<Rectangle>(3.0, 4.0));

    std::cout << "Vector of unique_ptr<Shape> (no slicing):\n";
    for (const auto& s : goodShapes) {
        std::cout << "  " << s->name() << " area = " << s->area() << "\n";
    }
}

// --- 6. Virtual Dispatch Performance Note ---

class PerformanceShape {
public:
    virtual ~PerformanceShape() = default;
    virtual double area() const { return 0.0; }
};

class PerformanceCircle : public PerformanceShape {
public:
    explicit PerformanceCircle(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; }
private:
    double r_;
};

void performanceNote() {
    std::cout << "\n--- Virtual Dispatch Cost ---\n";
    std::cout << "Virtual dispatch adds ~1 indirection per call.\n";
    std::cout << "For hot loops, consider:\n";
    std::cout << "  1. CRTP (static polymorphism)\n";
    std::cout << "  2. std::variant (value-based polymorphism)\n";
    std::cout << "  3. Inlining small functions\n";
}

int main() {
    virtualDispatchDemo();
    objectSlicingProblem();
    noSlicingDemo();
    polymorphismWithSmartPointers();
    slicingInContainers();
    performanceNote();

    std::cout << "\n=== All scenarios completed ===\n";
    return 0;
}
