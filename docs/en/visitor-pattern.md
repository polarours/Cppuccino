# Visitor Pattern

## What Is the Visitor Pattern?

The Visitor Pattern is a behavioral design pattern that lets you define a new
operation without changing the classes of the objects it operates on. It lets
you add new operations to an existing class hierarchy without modifying it.

> **Core idea**: separate the data structure from the operations on it, so new
> operations can be defined without changing the data structure.

## Why Do We Need It?

### Problem Scenario

Suppose we have a graphics editor that must support several shapes (circle,
rectangle, triangle) and several operations (draw, save, print):

```cpp
// Problem: adding a new operation means changing every class
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() = 0;
    virtual void save() = 0;
    virtual void print() = 0;  // adding a new operation means changing every class
};

class Circle : public Shape {
public:
    void draw() override { /* ... */ }
    void save() override { /* ... */ }
    void print() override { /* ... */ }
};

class Rectangle : public Shape {
public:
    void draw() override { /* ... */ }
    void save() override { /* ... */ }
    void print() override { /* ... */ }
};
```

**Problems**:

1. Adding a new operation means changing every class
2. Violates the open-closed principle
3. The classes carry too many responsibilities

### Solution

Use the visitor pattern:

```cpp
class Shape;  // forward declaration

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visitCircle(const Circle& circle) = 0;
    virtual void visitRectangle(const Rectangle& rectangle) = 0;
    virtual void visitTriangle(const Triangle& triangle) = 0;
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class Circle : public Shape {
public:
    void accept(Visitor& visitor) override {
        visitor.visitCircle(*this);
    }
};

// a new operation only needs a new Visitor subclass
class DrawVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        std::cout << "Drawing circle\n";
    }
    void visitRectangle(const Rectangle& rectangle) override {
        std::cout << "Drawing rectangle\n";
    }
    void visitTriangle(const Triangle& triangle) override {
        std::cout << "Drawing triangle\n";
    }
};
```

**Benefits**:

1. Easy to add new operations
2. Complies with the open-closed principle
3. Keeps the operation logic in one place

## Pattern Structure

```
                    ┌─────────────┐
                    │  Visitor    │
                    │ (interface) │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ DrawVisitor │ │ SaveVisitor │ │PrintVisitor │ ← concrete visitors
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │     Element             │ ← element interface
              │   (accept/visit)        │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │   Circle    │ │ Rectangle   │ │  Triangle   │ ← concrete elements
    └─────────────┘ └─────────────┘ └─────────────┘
```

## Implementation

### 1. Define the Visitor Interface

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>

class Shape;      // forward declaration
class Circle;     // forward declaration
class Rectangle;  // forward declaration
class Triangle;   // forward declaration

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visitCircle(const Circle& circle) = 0;
    virtual void visitRectangle(const Rectangle& rectangle) = 0;
    virtual void visitTriangle(const Triangle& triangle) = 0;
};
```

### 2. Define the Element Interface

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string name() const = 0;
};
```

### 3. Implement the Concrete Elements

```cpp
class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}
    
    void accept(Visitor& visitor) override {
        visitor.visitCircle(*this);
    }
    
    std::string name() const override { return "Circle"; }
    double getRadius() const { return radius_; }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height) 
        : width_(width), height_(height) {}
    
    void accept(Visitor& visitor) override {
        visitor.visitRectangle(*this);
    }
    
    std::string name() const override { return "Rectangle"; }
    double getWidth() const { return width_; }
    double getHeight() const { return height_; }

private:
    double width_;
    double height_;
};

class Triangle : public Shape {
public:
    Triangle(double a, double b, double c) 
        : a_(a), b_(b), c_(c) {}
    
    void accept(Visitor& visitor) override {
        visitor.visitTriangle(*this);
    }
    
    std::string name() const override { return "Triangle"; }
    double getA() const { return a_; }
    double getB() const { return b_; }
    double getC() const { return c_; }

private:
    double a_, b_, c_;
};
```

### 4. Implement the Concrete Visitors

```cpp
class DrawVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        std::cout << "Drawing circle with radius " << circle.getRadius() << "\n";
    }
    
    void visitRectangle(const Rectangle& rect) override {
        std::cout << "Drawing rectangle " << rect.getWidth() << "x" 
                  << rect.getHeight() << "\n";
    }
    
    void visitTriangle(const Triangle& tri) override {
        std::cout << "Drawing triangle (" << tri.getA() << ", " 
                  << tri.getB() << ", " << tri.getC() << ")\n";
    }
};

class SaveVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        std::cout << "Saving circle (radius: " << circle.getRadius() << ")\n";
    }
    
    void visitRectangle(const Rectangle& rect) override {
        std::cout << "Saving rectangle (" << rect.getWidth() << "x" 
                  << rect.getHeight() << ")\n";
    }
    
    void visitTriangle(const Triangle& tri) override {
        std::cout << "Saving triangle (" << tri.getA() << ", " 
                  << tri.getB() << ", " << tri.getC() << ")\n";
    }
};

class AreaVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        area_ += 3.14159 * circle.getRadius() * circle.getRadius();
    }
    
    void visitRectangle(const Rectangle& rect) override {
        area_ += rect.getWidth() * rect.getHeight();
    }
    
    void visitTriangle(const Triangle& tri) override {
        // Heron's formula
        double s = (tri.getA() + tri.getB() + tri.getC()) / 2;
        area_ += std::sqrt(s * (s - tri.getA()) * (s - tri.getB()) * (s - tri.getC()));
    }
    
    double getArea() const { return area_; }

private:
    double area_ = 0.0;
};
```

### 5. Client Usage

```cpp
int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));
    
    // draw all the shapes
    DrawVisitor drawVisitor;
    std::cout << "=== Drawing ===\n";
    for (const auto& shape : shapes) {
        shape->accept(drawVisitor);
    }
    
    // save all the shapes
    SaveVisitor saveVisitor;
    std::cout << "\n=== Saving ===\n";
    for (const auto& shape : shapes) {
        shape->accept(saveVisitor);
    }
    
    // compute the total area
    AreaVisitor areaVisitor;
    std::cout << "\n=== Area Calculation ===\n";
    for (const auto& shape : shapes) {
        shape->accept(areaVisitor);
    }
    std::cout << "Total area: " << areaVisitor.getArea() << "\n";
    
    return 0;
}
```

## Modern C++ Implementations

### Simplifying with Lambdas

```cpp
class LambdaVisitor {
public:
    template<typename T>
    void registerVisitor(std::function<void(const T&)> func) {
        visitors_[type_index<T>()] = [func](const auto& shape) {
            func(dynamic_cast<const T&>(shape));
        };
    }
    
    void visit(const Shape& shape) {
        auto it = visitors_.find(type_index(shape));
        if (it != visitors_.end()) {
            it->second(shape);
        }
    }

private:
    std::map<size_t, std::function<void(const Shape&)>> visitors_;
};
```

### Using CRTP

```cpp
template<typename Derived>
class VisitorBase {
public:
    void visit(Circle& circle) {
        static_cast<Derived*>(this)->visitCircle(circle);
    }
    
    void visit(Rectangle& rect) {
        static_cast<Derived*>(this)->visitRectangle(rect);
    }
    
    void visit(Triangle& tri) {
        static_cast<Derived*>(this)->visitTriangle(tri);
    }
};

class MyVisitor : public VisitorBase<MyVisitor> {
public:
    void visitCircle(Circle& circle) { /* ... */ }
    void visitRectangle(Rectangle& rect) { /* ... */ }
    void visitTriangle(Triangle& tri) { /* ... */ }
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Strategy** | Strategy switches algorithms; the visitor adds new operations |
| **Iterator** | An iterator traverses a collection; a visitor performs operations |
| **Command** | A command encapsulates a request; a visitor defines operations |

## Best Practices

### 1. Use Double Dispatch

```cpp
class Visitor;

class Element {
public:
    virtual void accept(Visitor& visitor) = 0;
};

class Visitor {
public:
    virtual void visitCircle(const Circle&) = 0;
    virtual void visitRectangle(const Rectangle&) = 0;
};

class Circle : public Element {
public:
    void accept(Visitor& visitor) override {
        visitor.visitCircle(*this);  // double dispatch
    }
};
```

### 2. Support Stateful Visitors

```cpp
class StatefulVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        currentShape_ = "Circle";
        // ...
    }
    
    std::string getCurrentShape() const { return currentShape_; }

private:
    std::string currentShape_;
};
```

### 3. Use Immutable Visitors

```cpp
class ImmutableVisitor {
public:
    virtual ~ImmutableVisitor() = default;
    virtual void visit(const Circle& circle) const = 0;
    virtual void visit(const Rectangle& rect) const = 0;
};
```

## Common Traps

### Trap 1: Breaking Encapsulation

```cpp
// Not recommended: the visitor needs access to private members
class BadVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        std::cout << circle.radius_;  // accessing a private member!
    }
};

// Recommended: go through the public interface
class GoodVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        std::cout << circle.getRadius();  // through a public method
    }
};
```

### Trap 2: An Overly Complex Visitor

```cpp
// Not recommended: the visitor takes on too many responsibilities
class BadVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        // too much logic
        calculateArea(circle);
        saveToFile(circle);
        sendNotification(circle);
        updateDatabase(circle);
    }
};

// Recommended: one visitor, one job
class AreaVisitor : public Visitor { ... };
class SaveVisitor : public Visitor { ... };
```

### Trap 3: Forgetting to Implement the visit Methods

```cpp
// Not recommended: forgetting some of the visit methods
class IncompleteVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override { /* ... */ }
    void visitRectangle(const Rectangle& rect) override { /* ... */ }
    // forgot visitTriangle!
};

// Recommended: let the IDE generate them, or use a static check
```

## Summary

The key points of the visitor pattern:

- **When to use it**: a stable data structure, operations that change often, a
  need to add new operations
- **Main benefits**: easy to extend, complies with the open-closed principle,
  operations stay centralized
- **Implementation advice**: double dispatch, go through the public
  interface, keep the single-responsibility principle
- **Common traps**: broken encapsulation, excessive complexity, forgotten
  implementations

> **Remember**: the key of the visitor pattern is *separating operations from
> the data structure* — add new operations without changing the data
> structure.
