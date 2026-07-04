# Object Lifetime and Destruction Order Pitfalls

## 1. Introduction

Understanding when objects are created and destroyed is fundamental to writing correct C++ code. Violations of lifetime rules lead to undefined behavior, including crashes, data corruption, and subtle bugs that are hard to reproduce.

## 2. Common Lifetime Issues

### 2.1 Dangling References to Local Variables

Returning or storing references to local variables creates dangling references:

```cpp
std::string& getGreeting() {
    std::string greeting = "Hello";
    return greeting;  // BAD: returning reference to local
}

std::string& ref = getGreeting();  // Dangling reference!
```

**Fix:** Return by value or use heap allocation.

### 2.2 Dangling Pointers After delete

Using a pointer after deleting the object it points to:

```cpp
int* p = new int(42);
delete p;
*p = 100;  // Undefined behavior!

// Even worse: double delete
delete p;  // Undefined behavior!
```

**Fix:** Set pointer to `nullptr` after delete, or use smart pointers.

### 2.3 Iterator Invalidation

Modifying a container invalidates its iterators:

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};
for (auto it = v.begin(); it != v.end(); ++it) {
    if (*it == 3) {
        v.erase(it);  // BAD: it is now invalid
    }
}
```

**Fix:** Use the iterator returned by `erase`, or use the erase-remove idiom.

## 3. Destruction Order Rules

### 3.1 Stack Objects

Stack objects are destroyed in reverse order of construction:

```cpp
{
    A a;  // Constructed first
    B b;  // Constructed second
    C c;  // Constructed third
    // Destroyed: c, b, a (reverse order)
}
```

### 3.2 Class Members

Members are destroyed in reverse order of declaration:

```cpp
class Widget {
    std::string name_;   // Destroyed second
    std::vector<int> data_;  // Destroyed first
};
```

### 3.3 Base Classes

Base classes are destroyed after derived classes:

```cpp
class Base {
public:
    ~Base() { std::cout << "Base destroyed\n"; }
};

class Derived : public Base {
public:
    ~Derived() { std::cout << "Derived destroyed\n"; }
};

Derived d;
// Output: Derived destroyed, then Base destroyed
```

## 4. Static Storage Duration

### 4.1 Static Initialization Order Fiasco

Static objects in different translation units may be initialized in an unpredictable order:

```cpp
// file1.cpp
extern int y;
int x = y + 1;  // BAD: y may not be initialized yet

// file2.cpp
int y = 42;
```

**Fix:** Use the "construct on first use" idiom:

```cpp
int& getY() {
    static int y = 42;
    return y;
}
```

### 4.2 Static Destruction Order

Static objects are destroyed in reverse order of construction within a translation unit, but across translation units the order is undefined:

```cpp
// This can cause problems:
static std::ofstream log("app.log");
// log may be destroyed before other static objects that use it
```

## 5. Thread Safety and Lifetime

### 5.1 Racing on Object Destruction

Accessing an object from multiple threads while it's being destroyed:

```cpp
std::thread t1([&]() { obj.method(); });
std::thread t2([&]() { obj.~Object(); });  // BAD!
```

**Fix:** Ensure the object outlives all threads accessing it.

### 5.2 Thread-Local Storage

Thread-local objects are destroyed when the thread exits:

```cpp
thread_local std::string name;
// name is destroyed when the thread finishes
```

## 6. Smart Pointer Lifetime

### 6.1 Circular References

```cpp
class Node {
    std::shared_ptr<Node> next;  // Strong reference
};

auto a = std::make_shared<Node>();
auto b = std::make_shared<Node>();
a->next = b;
b->next = a;  // Circular reference: memory leak!
```

**Fix:** Use `std::weak_ptr` to break cycles.

### 6.2 Custom Deleters

```cpp
auto deleter = [](File* f) {
    std::cout << "Closing file\n";
    fclose(f);
};

std::unique_ptr<FILE, decltype(deleter)> file(fopen("test.txt", "r"), deleter);
```

## 7. Best Practices

1. **Use RAII** — Bind resource lifetime to object lifetime
2. **Use smart pointers** — Prefer `unique_ptr` for single ownership
3. **Avoid raw new/delete** — Let containers and smart pointers manage memory
4. **Check for circular references** — Use `weak_ptr` to break cycles
5. **Understand destruction order** — Write predictable, well-structured code
6. **Thread safety** — Ensure objects outlive their threads
7. **Static objects** — Minimize dependencies between translation units

## 8. Summary

- Objects are destroyed in reverse order of construction
- Dangling references and pointers cause undefined behavior
- Static initialization order is undefined across translation units
- Smart pointers prevent most lifetime issues but have their own pitfalls
- Always think about object lifetime when designing APIs
