# Rvalue References

## What is an Rvalue Reference?

An rvalue reference (`T&&`) is a reference that binds to temporary objects — objects that are about to be destroyed, have no name, and typically appear on the right side of assignments.

```cpp
int x = 42;
int&& rref = 42;      // OK: 42 is an rvalue
int&& rref2 = x + 1;  // OK: x + 1 produces a temporary

// int&& bad = x;      // ERROR: x is an lvalue
```

## Key Properties

### 1. Binding Rules

Rvalue references bind to rvalues only (with special rules for forwarding references):

```cpp
int x = 10;
// int&& r1 = x;      // ERROR: x is an lvalue
int&& r2 = 42;        // OK: 42 is an rvalue
int&& r3 = x + 1;     // OK: temporary result

// Exception: forwarding references
template <typename T>
void wrapper(T&& arg);  // Can bind to both lvalues and rvalues
```

### 2. Enables Move Semantics

Rvalue references allow stealing resources from temporaries:

```cpp
class Buffer {
    int* data;
    size_t size;
public:
    Buffer(Buffer&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
};
```

### 3. Lifetime Extension

Temporary objects bound to rvalue references have extended lifetime:

```cpp
{
    const int&& ref = 42;  // Temporary lives until end of scope
    // ref is valid here
}
```

## Move Semantics

### The Problem

Before C++11, returning large objects from functions required expensive copies:

```cpp
std::vector<int> createVector() {
    std::vector<int> result(1000);
    // ... fill result
    return result;  // Expensive copy!
}
```

### The Solution

Move semantics allow transferring resources instead of copying:

```cpp
std::vector<int> createVector() {
    std::vector<int> result(1000);
    // ... fill result
    return result;  // Move (or NRVO) instead of copy
}
```

### std::move

`std::move` doesn't move anything — it just casts an lvalue to an rvalue reference:

```cpp
int x = 42;
int&& rref = std::move(x);  // Now x's resource can be moved from
```

## Common Use Cases

### Move Constructor

Transfer resources from one object to another:

```cpp
class SmartBuffer {
    int* data_;
    size_t size_;
public:
    SmartBuffer(SmartBuffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }
};
```

### Move Assignment Operator

```cpp
SmartBuffer& operator=(SmartBuffer&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}
```

### Perfect Forwarding

Preserve the value category of arguments:

```cpp
template <typename T>
void wrapper(T&& arg) {
    // Forward arg maintaining its value category
    target(std::forward<T>(arg));
}
```

## Pitfalls

### Using Moved-From Objects

After moving, the source object is in a valid but unspecified state:

```cpp
std::string s = "hello";
std::string moved = std::move(s);
// s is valid but empty; don't rely on its value
```

### Moving from const Objects

`std::move` on a const object produces a const rvalue reference, which falls back to copying:

```cpp
const std::string s = "hello";
std::string target = std::move(s);  // Copies, not moves!
```

### Moving Fundamental Types

Moving integers, pointers, etc. has no benefit:

```cpp
int x = 42;
int y = std::move(x);  // Same as copy
```

## Rvalue Reference vs Lvalue Reference

| Feature | Lvalue Reference (`T&`) | Rvalue Reference (`T&&`) |
|---------|------------------------|--------------------------|
| Binds to | Named objects | Temporaries |
| Rebinding | Not allowed | Not allowed |
| Primary use | Function parameters | Move semantics |
| Syntax | `T& ref = obj;` | `T&& ref = temp;` |

## Summary

- Rvalue references bind to temporary objects
- They enable move semantics for efficient resource transfer
- `std::move` casts to rvalue reference, enabling moves
- Use `noexcept` on move operations for exception safety
- Moved-from objects are valid but unspecified
