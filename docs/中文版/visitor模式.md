# 访问者模式 (Visitor Pattern)

## 什么是访问者模式？

访问者模式（Visitor Pattern）是一种行为型设计模式，它允许你定义一个新操作，而不用改变被操作对象的类。访问者模式使得可以在不修改现有类层次结构的情况下，向已有类层次结构添加新操作。

> **核心思想**：将数据结构与数据操作分离，使可以在不修改数据结构的情况下定义新的操作。

## 为什么需要访问者模式？

### 问题场景

假设我们有一个图形编辑程序，需要支持多种图形（圆形、矩形、三角形）和多种操作（绘制、保存、打印）：

```cpp
// 问题：添加新操作需要修改所有类
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() = 0;
    virtual void save() = 0;
    virtual void print() = 0;  // 添加新操作需要修改所有类
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

**问题**：
1. 添加新操作需要修改所有类
2. 违反开闭原则
3. 类职责过重

### 解决方案

使用访问者模式：

```cpp
class Shape;  // 前向声明

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

// 新操作只需新增 Visitor 子类
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

**优点**：
1. 易于添加新操作
2. 符合开闭原则
3. 集中管理操作逻辑

## 模式结构

```
                    ┌─────────────┐
                    │  Visitor    │
                    │  (访问者)    │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ DrawVisitor │ │ SaveVisitor │ │PrintVisitor │ ← 具体访问者
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │     Element             │ ← 元素接口
              │   (accept/visit)        │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │   Circle    │ │ Rectangle   │ │  Triangle   │ ← 具体元素
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义访问者接口

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <functional>

class Shape;  // 前向声明

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visitCircle(const Circle& circle) = 0;
    virtual void visitRectangle(const Rectangle& rectangle) = 0;
    virtual void visitTriangle(const Triangle& triangle) = 0;
};
```

### 2. 定义元素接口

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string name() const = 0;
};
```

### 3. 实现具体元素

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

### 4. 实现具体访问者

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

### 5. 客户端使用

```cpp
int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));
    
    // 绘制所有图形
    DrawVisitor drawVisitor;
    std::cout << "=== Drawing ===\n";
    for (const auto& shape : shapes) {
        shape->accept(drawVisitor);
    }
    
    // 保存所有图形
    SaveVisitor saveVisitor;
    std::cout << "\n=== Saving ===\n";
    for (const auto& shape : shapes) {
        shape->accept(saveVisitor);
    }
    
    // 计算总面积
    AreaVisitor areaVisitor;
    std::cout << "\n=== Area Calculation ===\n";
    for (const auto& shape : shapes) {
        shape->accept(areaVisitor);
    }
    std::cout << "Total area: " << areaVisitor.getArea() << "\n";
    
    return 0;
}
```

## 现代 C++ 实现

### 使用 lambda 简化

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

### 使用 CRTP

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

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **策略模式** | 策略模式切换算法；访问者模式添加新操作 |
| **迭代器模式** | 迭代器遍历集合；访问者执行操作 |
| **命令模式** | 命令封装请求；访问者定义操作 |

## 最佳实践

### 1. 使用双重分派

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
        visitor.visitCircle(*this);  // 双重分派
    }
};
```

### 2. 支持状态访问者

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

### 3. 使用不可变访问者

```cpp
class ImmutableVisitor {
public:
    virtual ~ImmutableVisitor() = default;
    virtual void visit(const Circle& circle) const = 0;
    virtual void visit(const Rectangle& rect) const = 0;
};
```

## 常见陷阱

### 陷阱 1：破坏封装性

```cpp
// 不推荐：访问者需要访问私有成员
class BadVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        std::cout << circle.radius_;  // 访问私有成员！
    }
};

// 推荐：通过公共接口访问
class GoodVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        std::cout << circle.getRadius();  // 通过公共方法
    }
};
```

### 陷阱 2：访问者过于复杂

```cpp
// 不推荐：访问者承担过多责任
class BadVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override {
        // 太多逻辑
        calculateArea(circle);
        saveToFile(circle);
        sendNotification(circle);
        updateDatabase(circle);
    }
};

// 推荐：每个访问者只做一件事
class AreaVisitor : public Visitor { ... };
class SaveVisitor : public Visitor { ... };
```

### 陷阱 3：忘记实现 visit 方法

```cpp
// 不推荐：忘记实现某些 visit 方法
class IncompleteVisitor : public Visitor {
public:
    void visitCircle(const Circle& circle) override { /* ... */ }
    void visitRectangle(const Rectangle& rect) override { /* ... */ }
    // 忘记 visitTriangle！
};

// 推荐：使用 IDE 自动生成或静态检查
```

## 总结

访问者模式的核心要点：

- **适用场景**：数据结构稳定、操作频繁变化、需要添加新操作
- **核心优点**：易于扩展、符合开闭原则、操作集中
- **实现要点**：双重分派、访问公共接口、保持单一职责
- **常见陷阱**：破坏封装、过于复杂、忘记实现

> **记住**：访问者模式的关键是"操作与数据结构分离"——在不修改数据结构的情况下添加新操作。
