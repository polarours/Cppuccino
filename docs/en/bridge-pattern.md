# Bridge Pattern

## What Is the Bridge Pattern?

The Bridge Pattern is a structural design pattern that separates the
abstraction from its implementation, so the two can vary independently.

> **Core idea**: bridge the abstraction to the implementation so that both
> can be extended without being tied to each other.

## Why Do We Need It?

### Problem Scenario

Suppose we have a graphics system that must support many shapes **and**
many rendering styles:

```cpp
// Problem: inheritance causes class explosion
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() = 0;
};

class Circle : public Shape { /* ... */ };
class Rectangle : public Shape { /* ... */ };
class Triangle : public Shape { /* ... */ };

// If we also need different colors
class RedCircle : public Circle { /* ... */ };
class BlueCircle : public Circle { /* ... */ };
class RedRectangle : public Rectangle { /* ... */ };
class BlueRectangle : public Rectangle { /* ... */ };
// number of shapes × number of colors = class explosion!
```

**Problems**:

1. The number of classes grows multiplicatively as more dimensions are added
2. Hard to maintain and extend
3. Violates the single-responsibility principle

### Solution

Use the bridge pattern:

```cpp
#include <memory>

// the implementation side (colors)
class Color {
public:
    virtual ~Color() = default;
    virtual void apply() const = 0;
};

class Red : public Color {
public:
    void apply() const override { /* paint red */ }
};

class Blue : public Color {
public:
    void apply() const override { /* paint blue */ }
};

// the abstraction side (shapes)
class ColoredShape {
public:
    explicit ColoredShape(std::unique_ptr<Color> color)
        : color_(std::move(color)) {}

    virtual ~ColoredShape() = default;
    virtual void draw() = 0;

protected:
    Color& getColor() const { return *color_; }

private:
    std::unique_ptr<Color> color_;
};

class ColoredCircle : public ColoredShape {
public:
    explicit ColoredCircle(std::unique_ptr<Color> color)
        : ColoredShape(std::move(color)) {}

    void draw() override {
        getColor().apply();
        // draw the circle...
    }
};

class ColoredRectangle : public ColoredShape {
public:
    explicit ColoredRectangle(std::unique_ptr<Color> color)
        : ColoredShape(std::move(color)) {}

    void draw() override {
        getColor().apply();
        // draw the rectangle...
    }
};
```

**Benefits**:

1. Decoupling: the abstraction and the implementation vary independently
2. Extensible: new shapes and new colors can be added separately
3. Fewer classes: N shapes + M colors = N + M classes

## Pattern Structure

```
                    ┌─────────────┐
                    │   Abstraction │
                    │  (Shape)     │
                    └──────┬──────┘
                           │ implemented by
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  Circle     │ │ Rectangle │ │ Triangle   │ ← extended abstraction
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │ bridged to
                           │
              ┌────────────▼────────────┐
              │      Implementor        │ ← the implementation interface
              │       (Color)           │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │    Red      │ │   Blue      │ │  Green     │ ← concrete implementations
    └─────────────┘ └─────────────┘ └─────────────┘
```

## Implementation

### 1. Define the Implementation Interface

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>

#include <memory>
#include <iostream>

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void renderCircle(double radius) = 0;
    virtual void renderRectangle(double width, double height) = 0;
    virtual void renderLine(double x1, double y1, double x2, double y2) = 0;
};

class VectorRenderer : public Renderer {
public:
    void renderCircle(double radius) override {
        std::cout << "Rendering vector circle with radius " << radius << "\n";
    }

    void renderRectangle(double width, double height) override {
        std::cout << "Rendering vector rectangle " << width << "x" << height << "\n";
    }

    void renderLine(double x1, double y1, double x2, double y2) override {
        std::cout << "Rendering vector line from (" << x1 << "," << y1
                  << ") to (" << x2 << "," << y2 << ")\n";
    }
};
```

### 2. Implement the Concrete Renderers

```cpp
class RasterRenderer : public Renderer {
public:
    void renderCircle(double radius) override {
        std::cout << "Rendering raster circle with radius " << radius << "\n";
    }

    void renderRectangle(double width, double height) override {
        std::cout << "Rendering raster rectangle " << width << "x" << height << "\n";
    }

    void renderLine(double x1, double y1, double x2, double y2) override {
        std::cout << "Rendering raster line from (" << x1 << "," << y1
                  << ") to (" << x2 << "," << y2 << ")\n";
    }
};

class PDFRenderer : public Renderer {
public:
    void renderCircle(double radius) override {
        std::cout << "Rendering PDF circle with radius " << radius << "\n";
    }

    void renderRectangle(double width, double height) override {
        std::cout << "Rendering PDF rectangle " << width << "x" << height << "\n";
    }

    void renderLine(double x1, double y1, double x2, double y2) override {
        std::cout << "Rendering PDF line from (" << x1 << "," << y1
                  << ") to (" << x2 << "," << y2 << ")\n";
    }
};
```

### 3. Define the Abstraction

```cpp
class Shape {
public:
    explicit Shape(std::unique_ptr<Renderer> renderer)
        : renderer_(std::move(renderer)) {}

    virtual ~Shape() = default;
    virtual void draw() = 0;

    // support runtime switching of the implementation
    void setRenderer(std::unique_ptr<Renderer> renderer) {
        renderer_ = std::move(renderer);
    }

protected:
    Renderer& getRenderer() const { return *renderer_; }

private:
    std::unique_ptr<Renderer> renderer_;
};

class Circle : public Shape {
public:
    Circle(std::unique_ptr<Renderer> renderer, double radius)
        : Shape(std::move(renderer)), radius_(radius) {}

    void draw() override {
        getRenderer().renderCircle(radius_);
    }

    void setRadius(double radius) { radius_ = radius; }
    double getRadius() const { return radius_; }

private:
    double radius_ = 0.0;
};

class Rectangle : public Shape {
public:
    Rectangle(std::unique_ptr<Renderer> renderer, double width, double height)
        : Shape(std::move(renderer)), width_(width), height_(height) {}

    void draw() override {
        getRenderer().renderRectangle(width_, height_);
    }

    void setSize(double width, double height) {
        width_ = width;
        height_ = height;
    }

    double getWidth() const { return width_; }
    double getHeight() const { return height_; }

private:
    double width_ = 0.0;
    double height_ = 0.0;
};
```

### 4. Client Usage

```cpp
int main() {
    // create renderers
    auto vectorRenderer = std::make_unique<VectorRenderer>();
    auto rasterRenderer = std::make_unique<RasterRenderer>();
    auto pdfRenderer = std::make_unique<PDFRenderer>();

    // create shapes with different renderers
    Circle vectorCircle(std::move(vectorRenderer), 5.0);
    Rectangle rasterRect(std::move(rasterRenderer), 10.0, 20.0);
    Circle pdfCircle(std::move(pdfRenderer), 8.0);

    // draw
    std::cout << "=== Vector Graphics ===\n";
    vectorCircle.draw();

    std::cout << "\n=== Raster Graphics ===\n";
    rasterRect.draw();

    std::cout << "\n=== PDF Graphics ===\n";
    pdfCircle.draw();

    // switch the implementation at runtime
    vectorCircle.setRenderer(std::make_unique<RasterRenderer>());
    std::cout << "\n=== Switched to Raster ===\n";
    vectorCircle.draw();

    return 0;
}
```

## Modern C++ Implementations

### Managing with Smart Pointers

```cpp
#include <memory>
#include <utility>

template <typename Renderer, typename... Args>
class ManagedShape {
public:
    explicit ManagedShape(Args&&... args)
        : renderer_(std::make_unique<Renderer>(std::forward<Args>(args)...)) {}

    void setRenderer(std::unique_ptr<Renderer> renderer) {
        renderer_ = std::move(renderer);
    }

private:
    std::unique_ptr<Renderer> renderer_;
};
```

### A Template Bridge

```cpp
#include <utility>

template <typename Renderer>
class TemplatedShape {
public:
    explicit TemplatedShape(Renderer renderer)
        : renderer_(std::move(renderer)) {}

    void drawCircle() {
        renderer_.renderCircle(1.0);
    }

private:
    Renderer renderer_;
};

void demoTemplated() {
    TemplatedShape<VectorRenderer> shape{VectorRenderer()};  // braces: avoid the vexing parse
    shape.drawCircle();
}
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Composite** | Composite is a tree of parts; Bridge separates abstraction from implementation |
| **Adapter** | Adapter converts one interface; Bridge decouples two dimensions of variation |
| **Decorator** | Decorator adds behavior incrementally; Bridge splits two independent axes |

## Best Practices

### 1. Choose the Bridge Point

```cpp
#include <memory>

// Recommended: bridge at the abstraction layer
class BridgedShape {
public:
    explicit BridgedShape(Renderer& renderer) : renderer_(renderer) {}

protected:
    Renderer& renderer_;  // the bridge point

    void draw() { renderer_.renderLine(0.0, 0.0, 1.0, 1.0); }
};

// Not recommended: bridge at the implementation layer
class DirectShape {
public:
    explicit DirectShape(Renderer& renderer) : renderer_(renderer) {}

    void draw() { renderer_.renderLine(0.0, 0.0, 1.0, 1.0); }

private:
    Renderer& renderer_;  // coupling is too tight
};
```

### 2. Use Dependency Injection

```cpp
class ShapeFactory {
public:
    static std::unique_ptr<Shape> createCircle(double radius,
                                               std::unique_ptr<Renderer> renderer) {
        return std::make_unique<Circle>(std::move(renderer), radius);
    }
};
```

### 3. Support Runtime Switching

```cpp
void demoSwitch() {
    auto shape = std::make_unique<Circle>(std::make_unique<VectorRenderer>(), 5.0);
    shape->draw();

    shape->setRenderer(std::make_unique<PDFRenderer>());
    shape->draw();  // now rendered through the PDF implementation
}
```

## Common Traps

### Trap 1: Bridging Too Early

```cpp
// Not recommended: bridging too early
class SimpleShape {
public:
    void draw() {
        // only one implementation exists — no bridge needed
    }
};

class BridgeShape {
public:
    void draw(Renderer& renderer) {
        // an early bridge just adds complexity
        (void)renderer;
    }
};

// Recommended: bridge only when there are real, independent axes of variation
```

### Trap 2: Too Many Bridge Objects

```cpp
#include <memory>

// Not recommended: every shape owns a separate renderer
class BadBridge {
public:
    explicit BadBridge(std::unique_ptr<Renderer> renderer)
        : renderer_(std::move(renderer)) {}

private:
    std::unique_ptr<Renderer> renderer_;  // one renderer per object
};

// Recommended: share a renderer (non-owning reference, initialized in the ctor)
class GoodBridge {
public:
    explicit GoodBridge(Renderer& renderer) : renderer_(renderer) {}

    void draw() { renderer_.renderLine(0.0, 0.0, 1.0, 1.0); }

private:
    Renderer& renderer_;  // shared renderer
};
```

### Trap 3: Forgetting to Manage the Lifecycle

```cpp
// Not recommended: a raw pointer
class BadShape {
public:
    void setRenderer(Renderer* renderer) {
        renderer_ = renderer;  // who deletes it?
    }

private:
    Renderer* renderer_ = nullptr;
};

// Recommended: a smart pointer
class GoodShape {
public:
    void setRenderer(std::unique_ptr<Renderer> renderer) {
        renderer_ = std::move(renderer);
    }

private:
    std::unique_ptr<Renderer> renderer_ = nullptr;
};
```

## Summary

The key points of the bridge pattern:

- **When to use it**: multiple independent axes of variation, both the
  abstraction and the implementation must be extended, or the
  implementation must be switchable at runtime
- **Main benefits**: decoupling, independent extension, fewer classes
- **Implementation advice**: choose the bridge point, use dependency
  injection, support runtime switching
- **Common traps**: bridging too early, too many bridge objects,
  lifecycle management

> **Remember**: the key of the bridge pattern is *separating variation* —
> split the abstraction from the implementation so each can change on its own.
