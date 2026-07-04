# 虚函数分发与对象模型

## C++ 对象模型基础

C++ 对象模型决定了对象在内存中的布局、虚函数如何调用、以及多态如何工作。理解这些底层机制对于编写高效、正确的代码至关重要。

## 对象内存布局

### 简单类

```cpp
class Point {
    int x;
    int y;
};
// sizeof(Point) == 8（通常）
// 内存布局：[x][y]
```

### 带虚函数的类

```cpp
class Shape {
    int id;
public:
    virtual double area() const;
    virtual void draw() const;
};
// 内存布局：[vptr][id]
// vptr 指向虚函数表（vtable）
```

### 单继承

```cpp
class Circle : public Shape {
    double radius;
};
// 内存布局：[vptr][id][radius]
// vptr 指向 Circle 的 vtable
```

### 多继承

```cpp
class Drawable {
public:
    virtual void draw() const;
};

class Circle : public Shape, public Drawable {
    double radius;
};
// 内存布局：[vptr1][id][vptr2][radius]
// 每个基类有自己的 vptr
```

## 虚函数表（vtable）

### 工作原理

```
Shape 的 vtable:
┌─────────────────────────────┐
│ &Shape::area                │
│ &Shape::draw                │
│ &Shape::~Shape              │
└─────────────────────────────┘

Circle 的 vtable:
┌─────────────────────────────┐
│ &Circle::area               │
│ &Circle::draw               │
│ &Circle::~Circle            │
└─────────────────────────────┘
```

### 虚函数调用过程

```cpp
Shape* shape = new Circle(5.0);
shape->area();  // 间接调用

// 编译器生成的代码大致如下：
// 1. 通过 vptr 找到 vtable
// 2. 在 vtable 中查找 area 函数指针
// 3. 调用该函数指针
```

## 虚函数分发

### 动态绑定

```cpp
class Base {
public:
    virtual void foo() { std::cout << "Base::foo\n"; }
};

class Derived : public Base {
public:
    void foo() override { std::cout << "Derived::foo\n"; }
};

Base* ptr = new Derived();
ptr->foo();  // 输出：Derived::foo（动态绑定）
```

### 静态绑定

```cpp
Base obj;
obj.foo();  // 输出：Base::foo（静态绑定，编译时确定）
```

### final 关键字

阻止进一步覆盖：

```cpp
class Final : public Derived {
public:
    void foo() final { std::cout << "Final::foo\n"; }
};

// class MoreFinal : public Final {
//     void foo() override;  // 错误：foo 是 final
// };
```

## 对象切片

### 问题

当派生类对象被赋值给基类对象时，派生类部分会被"切掉"：

```cpp
class Shape {
public:
    virtual double area() const { return 0; }
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area() const override { return 3.14159 * radius * radius; }
};

Circle c(5.0);
Shape s = c;  // 对象切片！Circle 部分丢失
s.area();     // 调用 Shape::area，返回 0
```

### 避免切片

使用指针或引用保持多态性：

```cpp
Circle c(5.0);
Shape& ref = c;      // OK：多态性保持
Shape* ptr = &c;     // OK：多态性保持
// Shape s = c;      // BAD：对象切片
```

## 虚析构函数

### 为什么需要

```cpp
class Base {
public:
    ~Base() { std::cout << "Base destroyed\n"; }
};

class Derived : public Base {
    int* data;
public:
    Derived() : data(new int[100]) {}
    ~Derived() { delete[] data; std::cout << "Derived destroyed\n"; }
};

Base* ptr = new Derived();
delete ptr;  // 只调用 ~Base()，Derived 的 data 泄漏！
```

### 解决方案

```cpp
class Base {
public:
    virtual ~Base() { std::cout << "Base destroyed\n"; }
};

delete ptr;  // 现在正确调用 ~Derived() 然后 ~Base()
```

## 性能考虑

### 虚函数开销

- 每个对象需要 vptr（通常 8 字节）
- 每次虚函数调用需要额外的间接寻址
- 无法内联虚函数调用

### 替代方案

```cpp
// CRTP（编译时多态）
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

// std::variant（值语义多态）
std::variant<Circle, Rectangle, Triangle> shape;
```

## 总结

- 虚函数通过 vtable 实现动态绑定
- 对象包含 vptr 指向其类的 vtable
- 对象切片发生在值赋值时，丢失派生类部分
- 虚析构函数是正确清理派生类资源的必要条件
- 理解对象模型有助于做出更好的设计决策
