# Ownership — Starting from Here

## The Starting Point of Object Lifetime Responsibility in C++
> In C++, almost all complex problems eventually come down to one question:
> "Who is responsible for this object?"

This article does not attempt to answer "how to write code", but rather: what **Ownership** is in C++ and why it matters.

## Why Talk About Ownership?

During the process of learning C++, we often encounter these questions:
- Does this object need to be `delete`d?
- When is the destructor called?
- Can this pointer be passed out?
- Why can an object be "used but shouldn't be used" after `move`?
- Why does `shared_ptr` have performance issues?
- Why can't `const` solve concurrent safety?

The root of these problems often relates to "who is responsible for the object's lifetime". Understanding ownership helps us write safer and more efficient code.

> C++ does not have a keyword called `ownership`, but it permeates the design philosophy and practice of C++. Understanding ownership helps us better understand C++'s memory management, resource management, and concurrency models.

## What is Ownership?

In the context of C++, ownership is not a syntactic concept but a relationship of responsibility.

More specifically, ownership means:
- Deciding when an object is **created** and when it is **destroyed**
- Ensuring the object remains valid when it is **used**
- Avoiding the object being **destroyed multiple times**

> Who is responsible for the object's lifetime owns it.
> C++ will not manage ownership for you, but forces you to be clear about responsibilities through its type system.

## In-depth Analysis of Ownership

### Types of Ownership

In C++, ownership can be divided into the following types:
- **Unique Ownership**: At any moment, a resource is allowed to have only one owner, with the lifetime responsibility being unique and clear.
A typical representative is `std::unique_ptr`, which is also the most recommended default ownership form in C++.
- **Shared Ownership**: Multiple objects share the responsibility for the resource's lifetime, with the resource destroyed when the last owner releases it.
A typical representative is `std::shared_ptr`, usually used in scenarios where a unique owner cannot be clearly identified.
- **Non-owning Observer**: Accessing a resource does not mean owning its lifetime, nor does it participate in the decision to destroy the resource.
Common forms include raw pointers, references, and `std::weak_ptr` (the latter is used to observe shared ownership objects).
- **Scope-bound Ownership**: The resource's lifetime is directly bound to a scope, with construction upon entering the scope and destruction upon leaving.
This is the basic form of `RAII` and widely present in stack objects and local resource management.
- **Ownership Transfer**: The responsibility for a resource's lifetime can be explicitly transferred between objects, usually implemented through move semantics.
This is a core capability in modern C++ API design.
- **Resource Ownership**: Ownership is not limited to memory but also includes system resources such as files, mutexes, and sockets.
The true value of `RAII` is often reflected in the management of these resources.

> It should be noted that the above classification is not mandatorily defined at the language level, but rather an abstract summary of common ownership semantics in C++ engineering practice.
> In engineering practice, a very important rule of thumb is: **prefer unique ownership first, and only consider shared ownership when a unique owner cannot be clearly identified.**
> In complex systems, more structural or context-dependent forms of ownership also appear, which will be gradually discussed in subsequent chapters.

### Ownership and Lifetime

Understanding ownership requires understanding object lifetime. Lifetime includes:
- **Creation**: When the object is allocated memory and initialized.
- **Usage**: The effective use of the object during its time in memory.
- **Destruction**: When the object is released from memory and its destructor is called.

Ownership determines who is responsible for managing these lifetime stages. Incorrect ownership management often leads to dangling pointers, memory leaks, double-free problems, etc.

More specific code can be found in [ownership_and_lifetime.cpp](../../examples/ownership-and-lifetime.cpp).

> More in-depth content about lifetime will be discussed in detail in subsequent chapters, tentatively titled **Ownership & Lifetime**.

## Best Practices for Ownership

### 1. Prefer Unique Ownership

```cpp
std::unique_ptr<int> p = std::make_unique<int>(42);
```

`std::unique_ptr` is the safest and most efficient form of ownership. It states plainly "I own this object", and it prevents accidental sharing.

### 2. Use std::string_view for Non-owning References

```cpp
void process(std::string_view data);
```

`std::string_view` provides non-owning access to a string and avoids an unnecessary copy.

### 3. Avoid Raw Pointer Ownership

```cpp
// Not recommended: the ownership of a raw pointer is unclear
int* p = new int(42);

// Recommended: use a smart pointer to make ownership explicit
auto p = std::make_unique<int>(42);
```

### 4. Make the Ownership Semantics of an Interface Explicit

```cpp
// Returning a unique_ptr: ownership transfer
std::unique_ptr<Foo> createFoo();

// Returning a const reference: non-owning observation
const Foo& getFoo();

// Taking a unique_ptr by value: ownership acquisition
void takeOwnership(std::unique_ptr<Foo> foo);
```

## Ownership and RAII

RAII (Resource Acquisition Is Initialization) is the core pattern of ownership management in C++. Its central idea is:

> **The lifetime of a resource is bound to the lifetime of an object.**

```cpp
class FileHandle {
    std::FILE* fp_;
public:
    explicit FileHandle(const char* filename) : fp_(std::fopen(filename, "r")) {}
    ~FileHandle() { if (fp_) std::fclose(fp_); }
    // No copy, move allowed
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    FileHandle(FileHandle&&) = default;
    FileHandle& operator=(FileHandle&&) = default;
};
```

Through RAII we can turn resource management into object management, and let constructors and destructors manage the lifetime of a resource automatically.

## Ownership and Concurrency

Ownership matters especially in concurrent programming.

### Thread Ownership

When an object is handed to another thread, ownership has to be transferred explicitly:

```cpp
void worker(std::unique_ptr<Data> data);
std::thread t(worker, std::make_unique<Data>(...));
t.join();
```

### Shared Ownership

When several threads need to reach the same object, consider shared ownership:

```cpp
std::shared_ptr<Data> sharedData = std::make_shared<Data>();
std::thread t1([](std::shared_ptr<Data> d) { /* ... */ }, sharedData);
std::thread t2([](std::shared_ptr<Data> d) { /* ... */ }, sharedData);
```

## Summary

Ownership is a core concept of C++. Understanding it helps us:
- Write safer and clearer code
- Avoid the common memory errors
- Design better APIs
- Manage resources more effectively

> **Remember: whoever is responsible owns it. Wherever ownership is unclear, bugs are waiting to happen.**
