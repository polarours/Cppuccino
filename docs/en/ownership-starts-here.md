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

For example:
```cpp
std::unique_ptr<int> ptr1(new int(42)); // ptr1 owns the resource
std::unique_ptr<int> ptr2 = std::move(ptr1); // Ownership transferred to ptr2
// ptr1 no longer owns the resource and can't be used anymore
```

More specific code can be found in [ownership_and_lifetime.cpp](/examples/ownership_and_lifetime.cpp).

> More in-depth content about lifetime will be discussed in detail in subsequent chapters, tentatively titled **Ownership & Lifetime**.

### Ownership and Concurrency

In multithreaded environments, ownership management becomes more complex. Incorrect ownership management can lead to data races, deadlocks, etc.
In concurrent contexts, ownership issues often directly amplify into memory model problems.
We know that concurrent programming usually needs to focus on three things:
1. Ensuring atomicity
2. Ensuring visibility
3. Ensuring ordering

Ownership management directly affects the implementation of these principles. For example, when using `std::shared_ptr`, the increment and decrement of reference counts must be atomic operations to prevent data races.

> More specific concurrent ownership management content will be discussed in detail in subsequent chapters, tentatively titled **Ownership & Concurrency**.

### Ownership and Performance
In C++, ownership is not a zero-cost abstraction. Different ownership models introduce different levels of runtime overhead during object construction, copying, destruction, and cross-thread usage.

- `std::unique_ptr` has very low overhead because it only needs to manage a pointer.
- `std::shared_ptr` needs to maintain a reference count, adding additional memory and computational overhead.

> More in-depth content about performance will be discussed in detail in subsequent chapters, tentatively titled **Ownership & Performance**.

### Ownership and Interface Design

Ownership management is a core consideration in C++ API design. A good interface design should clearly express the ownership semantics of resources, avoiding unclear responsibility divisions.

For example, the ownership semantics of function parameters and return values should be clear:
- Passing `std::unique_ptr` indicates ownership transfer.
- Passing raw pointers or references indicates non-owning observation.

> Ownership design at the interface level often determines the overall system's security and maintainability.
> More in-depth content on this topic will be discussed in subsequent chapters, tentatively titled **Ownership & API Design**.

## Summary

Ownership is not a specific feature of C++, but rather a relationship of responsibility that runs through the entire process of object creation, usage, and destruction.
C++ will not decide who owns an object for you, but forces you to be clear about this issue through its type system and semantic rules.

When we begin to examine code from the perspective of "who is responsible for the lifetime",
Many seemingly scattered issues — the timing of `delete`, the state of moved objects, the cost of `shared_ptr`, data security in concurrency — will naturally fall into place.

Understanding ownership is the starting point for understanding modern C++.
And truly mastering it often means you've started thinking about code in engineering terms rather than just syntactic ones.