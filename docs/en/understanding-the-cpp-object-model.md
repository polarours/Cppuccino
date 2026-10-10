# Understanding the C++ Object Model

## C++ Object Model Basics

The C++ object model dictates how objects are laid out in memory, how virtual
functions are called, and how polymorphism actually works. Understanding
these low-level mechanisms is essential for writing efficient, correct code.

## Object Memory Layout

### A Simple Class

```cpp
class Point {
    int x;
    int y;
};
// sizeof(Point) == 8 (typically)
// Layout: [x][y]
```

### A Class with Virtual Functions

```cpp
class Shape {
    int id;
public:
    virtual double area() const;
    virtual void draw() const;
};
// Layout: [vptr][id]
// vptr points to the virtual function table (vtable)
```

### Single Inheritance

```cpp
class Circle : public Shape {
    double radius;
};
// Layout: [vptr][id][radius]
// vptr points to Circle's vtable
```

### Multiple Inheritance

```cpp
class Drawable {
public:
    virtual void draw() const;
};

class Circle : public Shape, public Drawable {
    double radius;
};
// Layout: [vptr1][id][vptr2][radius]
// each base class gets its own vptr
```

## Virtual Function Table (vtable)

### How It Works

```
Shape's vtable:
+-----------------------------+
| &Shape::area                |
| &Shape::draw                |
| &Shape::~Shape              |
+-----------------------------+

Circle's vtable:
+-----------------------------+
| &Circle::area               |
| &Circle::draw               |
| &Circle::~Circle            |
+-----------------------------+
```

### The Call Sequence

```cpp
Shape* shape = new Circle(5.0);
shape->area();  // indirect call

// The compiler emits roughly:
// 1. load vptr from the object to find the vtable
// 2. look up the area function pointer in the vtable
// 3. call through that pointer
```

## Virtual Function Dispatch

### Dynamic Binding

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
ptr->foo();  // prints: Derived::foo (dynamic binding)
```

### Static Binding

```cpp
Base obj;
obj.foo();  // prints: Base::foo (static binding, resolved at compile time)
```

### The final Keyword

Blocks further overriding:

```cpp
class Final : public Derived {
public:
    void foo() final { std::cout << "Final::foo\n"; }
};

// class MoreFinal : public Final {
//     void foo() override;  // error: foo is final
// };
```

## Object Slicing

### The Problem

When a derived-class object is assigned to a base-class object, the derived
part gets "sliced" off:

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
Shape s = c;  // object slicing! The Circle part is lost
s.area();     // calls Shape::area, returns 0
```

### Avoiding Slicing

Keep polymorphism through pointers or references:

```cpp
Circle c(5.0);
Shape& ref = c;      // OK: polymorphism preserved
Shape* ptr = &c;     // OK: polymorphism preserved
// Shape s = c;      // BAD: object slicing
```

## Virtual Destructor

### Why You Need One

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
delete ptr;  // only ~Base() runs — Derived::data leaks!
```

### The Fix

```cpp
class Base {
public:
    virtual ~Base() { std::cout << "Base destroyed\n"; }
};

delete ptr;  // now correctly runs ~Derived() then ~Base()
```

## Performance Considerations

### Virtual Call Costs

- Every object needs a vptr (usually 8 bytes)
- Every virtual call costs an extra indirection
- Virtual calls cannot be inlined through a base pointer

### Alternatives

```cpp
// CRTP (compile-time polymorphism)
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

// std::variant (value-semantic polymorphism)
struct Square {};
struct Rectangle {};
struct Triangle {};
std::variant<Square, Rectangle, Triangle> shape;
```

## Summary

- Virtual functions implement dynamic binding through the vtable
- An object carries a vptr pointing at its class's vtable
- Object slicing happens on value assignment and drops the derived part
- A virtual destructor is mandatory for correct cleanup of derived resources
- Understanding the object model leads to better design decisions
