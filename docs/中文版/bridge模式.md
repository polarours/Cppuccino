# 桥接模式 (Bridge Pattern)

## 什么是桥接模式？

桥接模式（Bridge Pattern）是一种结构型设计模式，它将抽象部分与实现部分分离，使它们都可以独立地变化。

> **核心思想**：通过桥接将抽象和实现解耦，使两者可以独立扩展。

## 为什么需要桥接模式？

### 问题场景

假设我们有一个图形系统，需要支持多种形状和多种渲染方式：

```cpp
// 问题：继承导致类爆炸
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() = 0;
};

class Circle : public Shape { /* ... */ };
class Rectangle : public Shape { /* ... */ };
class Triangle : public Shape { /* ... */ };

// 如果需要不同颜色
class RedCircle : public Circle { /* ... */ };
class BlueCircle : public Circle { /* ... */ };
class RedRectangle : public Rectangle { /* ... */ };
class BlueRectangle : public Rectangle { /* ... */ };
// 形状数 × 颜色数 = 类数量爆炸！
```

**问题**：
1. 类数量随维度增加而指数增长
2. 难以维护和扩展
3. 违反单一职责原则

### 解决方案

使用桥接模式：

```cpp
// 实现部分（颜色）
class Color {
public:
    virtual ~Color() = default;
    virtual void apply() const = 0;
};

class Red : public Color {
public:
    void apply() const override { /* 应用红色 */ }
};

class Blue : public Color {
public:
    void apply() const override { /* 应用蓝色 */ }
};

// 抽象部分（形状）
class Shape {
public:
    explicit Shape(std::unique_ptr<Color> color)
        : color_(std::move(color)) {}
    
    virtual ~Shape() = default;
    virtual void draw() = 0;
    
protected:
    Color& getColor() const { return *color_; }

private:
    std::unique_ptr<Color> color_;
};

class Circle : public Shape {
public:
    explicit Circle(std::unique_ptr<Color> color)
        : Shape(std::move(color)) {}
    
    void draw() override {
        getColor().apply();
        // 绘制圆形...
    }
};

class Rectangle : public Shape {
public:
    explicit Rectangle(std::unique_ptr<Color> color)
        : Shape(std::move(color)) {}
    
    void draw() override {
        getColor().apply();
        // 绘制矩形...
    }
};
```

**优点**：
1. 解耦：抽象和实现独立变化
2. 可扩展：可以独立添加新形状或新颜色
3. 减少类数量：N 个形状 + M 个颜色 = N+M 个类

## 模式结构

```
                    ┌─────────────┐
                    │   Abstraction │
                    │  (Shape)     │
                    └──────┬──────┘
                           │ implements
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  Circle     │ │ Rectangle │ │ Triangle   │ ← 扩充抽象
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │ bridges
                           │
              ┌────────────▼────────────┐
              │      Implementor        │ ← 实现接口
              │       (Color)           │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │    Red      │ │   Blue      │ │  Green     │ ← 具体实现
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义实现接口

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void renderCircle(double radius) = 0;
    virtual void renderRectangle(double width, double height) = 0;
    virtual void renderLine(double x1, double y1, double x2, double y2) = 0;
};
```

### 2. 实现具体渲染器

```cpp
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

### 3. 定义抽象部分

```cpp
class Shape {
public:
    explicit Shape(std::unique_ptr<Renderer> renderer)
        : renderer_(std::move(renderer)) {}
    
    virtual ~Shape() = default;
    virtual void draw() = 0;
    
protected:
    Renderer& getRenderer() const { return *renderer_; }

private:
    std::unique_ptr<Renderer> renderer_;
};

class Circle : public Shape {
public:
    explicit Circle(std::unique_ptr<Renderer> renderer, double radius)
        : Shape(std::move(renderer)), radius_(radius) {}
    
    void draw() override {
        renderer_.get().renderCircle(radius_);
    }
    
    void setRadius(double radius) { radius_ = radius; }
    double getRadius() const { return radius_; }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(std::unique_ptr<Renderer> renderer, double width, double height)
        : Shape(std::move(renderer)), width_(width), height_(height) {}
    
    void draw() override {
        renderer_.get().renderRectangle(width_, height_);
    }
    
    void setSize(double width, double height) {
        width_ = width;
        height_ = height;
    }
    
    double getWidth() const { return width_; }
    double getHeight() const { return height_; }

private:
    double width_;
    double height_;
};
```

### 4. 客户端使用

```cpp
int main() {
    // 创建渲染器
    auto vectorRenderer = std::make_unique<VectorRenderer>();
    auto rasterRenderer = std::make_unique<RasterRenderer>();
    auto pdfRenderer = std::make_unique<PDFRenderer>();
    
    // 创建图形，使用不同的渲染器
    Circle vectorCircle(std::move(vectorRenderer), 5.0);
    Rectangle rasterRect(std::move(rasterRenderer), 10.0, 20.0);
    Circle pdfCircle(std::move(pdfRenderer), 8.0);
    
    // 绘制
    std::cout << "=== Vector Graphics ===\n";
    vectorCircle.draw();
    
    std::cout << "\n=== Raster Graphics ===\n";
    rasterRect.draw();
    
    std::cout << "\n=== PDF Graphics ===\n";
    pdfCircle.draw();
    
    // 动态切换渲染器
    vectorCircle = Circle(std::make_unique<RasterRenderer>(), 5.0);
    std::cout << "\n=== Switched to Raster ===\n";
    vectorCircle.draw();
    
    return 0;
}
```

## 现代 C++ 实现

### 使用智能指针管理

```cpp
class ManagedShape {
public:
    template<typename Renderer, typename... Args>
    explicit ManagedShape(Args&&... args)
        : renderer_(std::make_unique<Renderer>(std::forward<Args>(args)...)) {}
    
    void setRenderer(std::unique_ptr<Renderer> renderer) {
        renderer_ = std::move(renderer);
    }

private:
    std::unique_ptr<Renderer> renderer_;
};
```

### 使用模板桥接

```cpp
template<typename Renderer>
class TemplatedShape {
public:
    explicit TemplatedShape(Renderer renderer)
        : renderer_(std::move(renderer)) {}
    
    void draw() {
        renderer_.render();
    }

private:
    Renderer renderer_;
};

// 使用
TemplatedShape<VectorRenderer> shape(VectorRenderer());
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **组合模式** | 组合模式树形结构；桥接模式分离抽象和实现 |
| **适配器模式** | 适配器转换接口；桥接模式分离抽象和实现 |
| **装饰器模式** | 装饰器增强功能；桥接模式分离变化维度 |

## 最佳实践

### 1. 选择合适的桥接点

```cpp
// 推荐：在抽象层桥接
class Shape {
protected:
    Renderer& renderer_;  // 桥接点
};

// 不推荐：在实现层桥接
class Circle {
    Renderer& renderer_;  // 耦合太紧
};
```

### 2. 使用依赖注入

```cpp
class ShapeFactory {
public:
    static std::unique_ptr<Shape> createCircle(double radius, 
                                                std::unique_ptr<Renderer> renderer) {
        return std::make_unique<Circle>(std::move(renderer), radius);
    }
};
```

### 3. 支持运行时切换

```cpp
class SwitchableShape : public Shape {
public:
    void setRenderer(std::unique_ptr<Renderer> renderer) {
        renderer_ = std::move(renderer);
    }
    
    void draw() override {
        renderer_.get().render();
    }
};
```

## 常见陷阱

### 陷阱 1：桥接过早

```cpp
// 不推荐：过早桥接
class SimpleShape {
public:
    void draw() {
        // 只有一个实现，不需要桥接
    }
};

class BridgeShape {
public:
    void draw(Renderer& renderer) {
        // 过早桥接，增加复杂度
    }
};

// 推荐：在有多个变化维度时桥接
```

### 陷阱 2：桥接对象过多

```cpp
// 不推荐：每个形状都有独立的渲染器
class BadBridge {
    std::unique_ptr<Renderer> renderer_;  // 每个对象都持有
};

// 推荐：共享渲染器
class GoodBridge {
    Renderer& renderer_;  // 引用共享渲染器
};
```

### 陷阱 3：忘记管理生命周期

```cpp
// 不推荐：裸指针
class BadShape {
public:
    void setRenderer(Renderer* renderer) {
        renderer_ = renderer;  // 谁负责 delete？
    }
    
private:
    Renderer* renderer_ = nullptr;
};

// 推荐：智能指针
class GoodShape {
public:
    void setRenderer(std::unique_ptr<Renderer> renderer) {
        renderer_ = std::move(renderer);
    }
    
private:
    std::unique_ptr<Renderer> renderer_;
};
```

## 总结

桥接模式的核心要点：

- **适用场景**：多个变化维度、抽象和实现都需要扩展、需要运行时切换
- **核心优点**：解耦、独立扩展、减少类数量
- **实现要点**：选择合适的桥接点、使用依赖注入、支持运行时切换
- **常见陷阱**：过早桥接、对象过多、生命周期管理

> **记住**：桥接模式的关键是"分离变化"——将抽象和实现分离，使它们可以独立变化。
