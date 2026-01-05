# Rvalue References (T&&): Not "Advanced References", but a **Permission Mechanism**

`std::move` does not actually move objects; it merely converts an expression to an **rvalue reference**, thereby allowing **safe transfer** of resources.
This article will deeply explain the nature of rvalue references and the problems they solve.

> **Tip**: For detailed definitions of lvalue references, see the [Lvalue References](/docs/en/lvalue-references.md) chapter.

## What are rvalue references? Why do we need them?

Before C++11, the language lacked a safe, standard, and non-destructive way to express ownership transfer of objects. We say "lacked" instead of "could not express" because there were imperfect solutions like `std::auto_ptr`, which will be discussed later.

Consider the following scenario:

```cpp
void push_back(const T& value) {
    data_[size_++] = value;
}
```

The problem is obvious:
- Every `push_back` call **copies** the passed object `value`, which is inefficient when handling large objects.
- If we could **move** the object's resources instead of copying them, we could significantly improve performance.

To solve this problem, C++11 introduced rvalue references (T&&). Before this, C++ lacked a language-level, general, and semantically clear mechanism to avoid unnecessary copies; developers could only rely on compiler optimizations or manual techniques, struggling between maintainability and performance.
Rvalue references allow us to distinguish between **temporary objects** (rvalues) and **persistent objects** (lvalues), thereby enabling **move semantics**.
In one sentence: rvalue references are not designed to solve the "copying is slow" problem, but to solve the "the language cannot express ownership transfer" problem.

> Tip: For detailed definitions of rvalue references, see section 3.10 "Lvalues and rvalues" of the C++11 standard draft N3337 "Working Draft, Standard for Programming Language C++".

## Deep dive into rvalue references
The syntax for rvalue references is `T&&`, which expresses a semantic intent: the object no longer bears subsequent responsibility, and its resources can be safely transferred. In contrast, we also have **lvalue references** (`T&`). The differences between them are as follows:

- **Lvalue reference** (`T&`) binds to persistent objects that can be used multiple times.
- **Rvalue reference** (`T&&`) can bind to rvalue expressions, expressing the semantics that: the object's lifetime is about to end, or its resources can be safely transferred.

> Tip: ~~For a detailed explanation of lvalue references, see the related chapter (to be added).~~ For the chapter on lvalue references, see [Lvalue References](/docs/en/lvalue-references.md).

### 1. The essence of the problem: Can the object "still be used" or not?
In C++, almost all performance issues ultimately come down to object lifetime and ownership management.
Let's start with a simple example:

```cpp
std::string createString() {
    std::string str = "Hello, World!";
    return str; // Return a local variable
}
```

In this example, `str` is a local variable. When the `createString` function returns, the local variable `str`'s lifetime ends.
So since it can no longer be used, why not just "take" its resources? This is the core idea of rvalue references.

### 2. Semantics of rvalue references: Allowing resource "to be safely transferred"
Unlike lvalue references, rvalue references allow us to "safely transfer" resources from temporary objects rather than copying them. For example:

Same example, for lvalue reference:

```cpp
void push_back(const std::string& value) {
    data_[size_++] = value; // Copy value through copy constructor
}
```

For rvalue reference:

```cpp
void push_back(std::string&& value) {
    data_[size_++] = std::move(value); // "Safely transfer" value's resources through move constructor
}
```

So, the difference between "lvalue" and "rvalue" ultimately comes down to whether the object's lifetime and ownership can be transferred.
At an intuitive level, "has a name or not" can help beginners quickly distinguish lvalues from rvalues (though not the language definition); but at the language level, the more essential difference is: whether the expression represents an object state that can be continuously relied upon.

> It's important to emphasize: whether resource transfer actually occurs still completely depends on the type's move constructor / move assignment implementation, not on the rvalue reference itself.

### 3. Before rvalue references, how did C++ handle this problem?
Before C++11, it wasn't completely impossible to reduce copying, but there was no mechanism directly expressing "transferable ownership" in the type system, resulting in all solutions either relying on optimizations or sacrificing intuitive interface design. To optimize performance, developers often needed to manually write code to avoid copying, or rely on compiler optimizations like RVO/NRVO (Return Value Optimization/Named Return Value Optimization).

#### 3.1 RVO/NRVO
RVO (Return Value Optimization) and NRVO (Named Return Value Optimization) are compiler optimization techniques that allow the compiler to construct return objects directly in the caller's memory space, thereby avoiding unnecessary copy operations. However, this optimization is not mandatory; the compiler can choose not to apply it.

```cpp
std::string createString() {
    std::string str = "Hello, World!";
    return str; // Compiler may apply RVO/NRVO optimization
}
```

#### 3.2 Pass by reference
Another common approach is to pass object references to avoid copying:

```cpp
void processString(const std::string& str) {
    // Process string
}
```

#### 3.3 Using output parameters / in-place construction to avoid intermediate objects
Sometimes, developers use output parameters or in-place construction to avoid creating intermediate objects:

```cpp
void createString(std::string& outStr) {
    outStr = "Hello, World!"; // Construct directly on output parameter
}
```

### 4. Ownership transfer with pointers/handles: std::auto_ptr (historical), raw pointers, smart pointers (Boost)

Before C++11, managing dynamically allocated resources (like memory) typically relied on raw pointers or smart pointers (like Boost's `shared_ptr` and `scoped_ptr`). However, these methods did not directly solve the problem of object ownership transfer.

#### 4.1 `std::auto_ptr`

`std::auto_ptr` was a smart pointer introduced in C++98 designed to manage dynamically allocated resources. It implemented ownership transfer by overloading the copy constructor and assignment operator. However, `std::auto_ptr` had some issues, such as causing unexpected behavior when used in containers. Considering everything, it was deprecated in C++11 and replaced by `std::unique_ptr`.

```cpp
std::auto_ptr<MyClass> ptr1(new MyClass());
std::auto_ptr<MyClass> ptr2 = ptr1; // Ownership transfer
```

> `std::auto_ptr` was not move semantics, but rather a historical compromise where "the copy interface was forced to bear the responsibility of ownership transfer semantics." This is also one of the direct motivations for C++11 to explicitly distinguish copy from move.

#### 4.2 Raw pointers: Manual Ownership Transfer

In the era without smart pointers, ownership transfer completely relied on the programmer's discipline and documentation. The most common patterns were "factory functions" or "takeover functions".

```cpp
// Pattern A: Transfer ownership through function return value
MyClass* createObject() {
    return new MyClass(); // Caller is responsible for deleting the object
}

// Pattern B: Explicit manual transfer
MyClass* ptr1 = new MyClass();
MyClass* ptr2 = ptr1; // At this point, both ptr1 and ptr2 point to it, ownership is ambiguous
ptr1 = NULL;          // Programmer manually sets the original pointer to null, simulating "move"
```

Limitations: This approach is highly prone to memory leaks (forgetting to delete) or dangling pointers (multiple locations holding the pointer, with one releasing the memory).

#### 4.3 Boost Library: Clear Ownership Semantics

Before C++11, the Boost library was the de facto standard, clarifying ownership through different smart pointers.

##### A. boost::scoped_ptr — No Transfer Allowed

It expresses "absolute ownership that cannot be transferred". It disabled copy constructor and assignment operator.

```cpp
#include <boost/scoped_ptr.hpp>

void example() {
    boost::scoped_ptr<MyClass> ptr(new MyClass());
    // boost::scoped_ptr<MyClass> ptr2 = ptr; // Compilation error! Transfer or copy not allowed
} // At function end, ptr automatically releases memory
```

##### B. boost::shared_ptr — Shared Ownership

It implements "joint ownership" through reference counting. This was the most common solution in C++98 to solve ownership chaos, although it doesn't involve "transfer" but rather "adding owners".

```cpp
#include <boost/shared_ptr.hpp>

void example() {
    boost::shared_ptr<MyClass> ptr1(new MyClass());
    boost::shared_ptr<MyClass> ptr2 = ptr1; // Reference count increases by 1
} // When ptr1, ptr2 are destructed, memory is actually released only when count reaches 0
```

#### 5. Copy-on-Write (COW) / Reference-counted shared representation
Another technique commonly used before C++11 was Copy-on-Write (COW) and reference-counted shared representation. This technique allows multiple objects to share the same data, only performing a copy when the data needs to be modified, thereby reducing unnecessary copy operations.

```cpp
class String {
public:
    // Reference-counted shared representation
};
```

The cost of this approach is increased complexity and runtime overhead, as well as thread safety issues.

> The above solutions are not perfect and have various issues, such as complex semantics and performance overhead. C++11 introduced rvalue references and move semantics to solve these problems, providing a more intuitive and efficient ownership transfer mechanism.

## T& / const T& / T&&: When to use which?

In C++, understanding when to use lvalue references (`T&`), constant lvalue references (`const T&`), and rvalue references (`T&&`) is crucial for writing efficient and semantically clear code. Here are their respective use cases:

- `T&` Borrowing, modifications will affect the original object.
- `const T&` Read-only, safely borrow persistent or temporary objects.
- `T&&` Transferable ownership, suitable for temporary objects or named objects converted by `std::move`.

Here are some specific examples:

### 1. If the function parameter is an object and you want to be able to modify it, use `T&`

```cpp
void foo(T& obj) {
    // Modify obj
}
foo(existingObject); // Pass a persistent object
foo(T()); // Error, cannot pass a temporary object
```

> The implication of lvalue reference is "I'm referencing an object that will continue to be used, and my modifications to it will affect its subsequent use."

### 2. If the function parameter is an object and you don't want to modify it, use `const T&`

```cpp
void foo(const T& obj) {
    // Read-only access to obj
}
foo(existingObject); // Pass a persistent object
foo(T()); // Pass a temporary object
```

> The conventional way to pass objects, suitable for most scenarios.

### 3. As mentioned above: If the function parameter is an object and you want to be able to "safely transfer" its resources, use `T&&`

```cpp
void foo(T&& obj) {
    // Steal obj's resources
}
foo(T()); // Pass a temporary object, the most natural usage
foo(existingObject); // Error, cannot pass a persistent object
foo(std::move(existingObject)); // Correct, convert to rvalue reference via std::move, for named objects using rvalue reference
```

> The implication of rvalue reference is "I'm referencing an object that is about to be destroyed, and I can safely transfer its resources because it won't be used again."
> Of course, "about to be destroyed" is not quite accurate. A more precise way to put it is "the object's task is complete, and it no longer bears responsibility."

### Summary

- `T&`: I borrow your thing (you'll use it later), I might modify it
- `const T&`: I'm just looking (you'll use it later), I won't modify it
- `T&&`: You're ready to give it up, so I'll take the usable resources (consume)

By understanding the essence and use cases of rvalue references, developers can more effectively manage object lifetimes and resources, thereby improving program performance and maintainability. I hope this helps everyone better understand this important feature introduced in C++11.If I were to summarize the role of `std::move` in one sentence, it would be: "Let C++ distinguish at the language level between 'end of lifecycle' and 'resource reuse'."
