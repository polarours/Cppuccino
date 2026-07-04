// examples/pitfall-object-slicing.cpp
// Demonstrates object slicing pitfalls and their fixes.
// Compile with: g++ -std=c++17 -o pitfall_slicing pitfall-object-slicing.cpp

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// --- 1. Basic Object Slicing ---

class Shape {
public:
    Shape() = default;
    Shape(const Shape&) = default;
    Shape& operator=(const Shape&) = default;
    virtual ~Shape() = default;

    virtual double area() const { return 0.0; }
    virtual std::string type() const { return "Shape"; }
};

class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}
    double area() const override { return 3.14159 * radius_ * radius_; }
    std::string type() const override { return "Circle"; }
private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}
    double area() const override { return width_ * height_; }
    std::string type() const override { return "Rectangle"; }
private:
    double width_, height_;
};

void basicObjectSlicing() {
    std::cout << "--- Basic Object Slicing ---\n";

    Circle c(5.0);

    // BAD: Slicing occurs
    Shape sliced = c;  // Circle part is sliced off!
    std::cout << "Sliced: " << sliced.type() << " area=" << sliced.area() << "\n";

    // GOOD: Use pointer or reference
    Shape& ref = c;
    std::cout << "Reference: " << ref.type() << " area=" << ref.area() << "\n";

    Shape* ptr = &c;
    std::cout << "Pointer: " << ptr->type() << " area=" << ptr->area() << "\n";
}

// --- 2. Slicing in Function Parameters ---

void processSliced(Shape s) {
    std::cout << "ProcessSliced: " << s.type() << " area=" << s.area() << "\n";
}

void processRef(const Shape& s) {
    std::cout << "ProcessRef: " << s.type() << " area=" << s.area() << "\n";
}

void slicingInFunctionParameters() {
    std::cout << "\n--- Slicing in Function Parameters ---\n";

    Circle c(3.0);

    // BAD: Slicing
    processSliced(c);

    // GOOD: Use reference
    processRef(c);
}

// --- 3. Slicing in Containers ---

void slicingInContainers() {
    std::cout << "\n--- Slicing in Containers ---\n";

    // BAD: vector<Shape> causes slicing
    std::vector<Shape> badShapes;
    badShapes.push_back(Circle(5.0));
    badShapes.push_back(Rectangle(3.0, 4.0));

    std::cout << "Bad (sliced): ";
    for (const auto& s : badShapes) {
        std::cout << s.type() << " ";
    }
    std::cout << "\n";

    // GOOD: Use smart pointers
    std::vector<std::unique_ptr<Shape>> goodShapes;
    goodShapes.push_back(std::make_unique<Circle>(5.0));
    goodShapes.push_back(std::make_unique<Rectangle>(3.0, 4.0));

    std::cout << "Good (polymorphic): ";
    for (const auto& s : goodShapes) {
        std::cout << s->type() << " ";
    }
    std::cout << "\n";
}

// --- 4. Slicing with Assignment ---

void slicingWithAssignment() {
    std::cout << "\n--- Slicing with Assignment ---\n";

    Circle c1(2.0);
    Circle c2(3.0);

    // BAD: Slicing if assigning to base
    Shape s1 = c1;
    Shape s2 = c2;
    s1 = s2;  // Assigns Shape part only

    std::cout << "After assignment: " << s1.type() << " area=" << s1.area() << "\n";

    // GOOD: Use references
    Shape& r1 = c1;
    Shape& r2 = c2;
    // r1 = r2;  // This would modify c1, not rebind r1
}

// --- 5. Slicing in Return Values ---

Shape badReturnShape() {
    Circle c(4.0);
    return c;  // BAD: Slicing
}

Shape& badReturnRef() {
    Circle c(4.0);  // BAD: returning reference to local
    return c;
}

void slicingInReturnValues() {
    std::cout << "\n--- Slicing in Return Values ---\n";

    // BAD: Slicing
    Shape s = badReturnShape();
    std::cout << "Sliced return: " << s.type() << " area=" << s.area() << "\n";

    // GOOD: Return by pointer or use factory pattern
    // (See fix below)
}

// --- 6. Fix: Factory Pattern ---

std::unique_ptr<Shape> createShape(const std::string& type, double param) {
    if (type == "circle") {
        return std::make_unique<Circle>(param);
    } else if (type == "rectangle") {
        return std::make_unique<Rectangle>(param, param * 0.75);
    }
    return std::make_unique<Shape>();
}

void fixFactoryPattern() {
    std::cout << "\n--- Fix: Factory Pattern ---\n";

    auto c = createShape("circle", 5.0);
    auto r = createShape("rectangle", 4.0);

    std::cout << "Circle: " << c->type() << " area=" << c->area() << "\n";
    std::cout << "Rectangle: " << r->type() << " area=" << r->area() << "\n";
}

// --- 7. Fix: Virtual Clone ---

class ShapeCloneable : public Shape {
public:
    virtual std::unique_ptr<ShapeCloneable> clone() const = 0;
};

class CircleCloneable : public ShapeCloneable {
public:
    explicit CircleCloneable(double r) : radius_(r) {}
    double area() const override { return 3.14159 * radius_ * radius_; }
    std::string type() const override { return "Circle"; }
    std::unique_ptr<ShapeCloneable> clone() const override {
        return std::make_unique<CircleCloneable>(radius_);
    }
private:
    double radius_;
};

void fixVirtualClone() {
    std::cout << "\n--- Fix: Virtual Clone ---\n";

    CircleCloneable original(3.0);
    auto copy = original.clone();  // No slicing!

    std::cout << "Original: " << original.type() << " area=" << original.area() << "\n";
    std::cout << "Copy: " << copy->type() << " area=" << copy->area() << "\n";
}

// --- 8. Slicing Detection at Compile Time ---

// This function will cause a compile error if someone tries to slice
void noSliceAllowed(const Shape&) = delete;

void slicingDetection() {
    std::cout << "\n--- Slicing Detection ---\n";

    Circle c(5.0);

    // This would cause compile error:
    // noSliceAllowed(c);

    std::cout << "Compile-time slicing prevention available\n";
}

int main() {
    basicObjectSlicing();
    slicingInFunctionParameters();
    slicingInContainers();
    slicingWithAssignment();
    slicingInReturnValues();
    fixFactoryPattern();
    fixVirtualClone();
    slicingDetection();

    std::cout << "\n=== All object slicing scenarios completed ===\n";
    return 0;
}
