# Rvalue References (T&&): Not "Advanced References", but a **Permission Mechanism**

In the move-is-not-moving chapter, we mentioned that `std::move` does not "move" anything; it simply converts an expression to an rvalue reference, thereby **allowing** subsequent code to choose move semantics.  
In this article, we'll fill in that "to be added" note: **What exactly are rvalue references? What problem do they solve?**

> **Tip**: For detailed definitions of lvalue references, see the [Lvalue References](/docs/en/lvalue-references.md) chapter.

## What are rvalue references? Why do we need them?
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

> Tip: For a detailed explanation of lvalue references, see the related chapter (to be added).

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

### 2. Semantics of rvalue references: Allowing resource "theft"
Unlike lvalue references, rvalue references allow us to "steal" resources from temporary objects rather than copying them. For example:

Same example, for lvalue reference:

```cpp
void push_back(const std::string& value) {
    data_[size_++] = value; // Copy value through copy constructor
}
```

For rvalue reference:

```cpp
void push_back(std::string&& value) {
    data_[size_++] = std::move(value); // "Steal" value's resources through move constructor
}
```

So, the difference between "lvalue" and "rvalue" ultimately comes down to whether the object's lifetime and ownership can be transferred.
At an intuitive level, "has a name or not" can help beginners quickly distinguish lvalues from rvalues (though not the language definition); but at the language level, the more essential difference is: whether the expression represents an object state that can be continuously relied upon.

> It's important to emphasize: whether resource transfer actually occurs still completely depends on the type's move constructor / move assignment implementation, not on the rvalue reference itself.

### 3. Before rvalue references, how did C++ handle this problem?
Before C++11, it wasn't completely impossible to reduce copying, but there was no mechanism directly expressing "transferable ownership" in the type system, resulting in all solutions either relying on optimizations or sacrificing intuitive interface design. To optimize performance, developers often needed to manually write code to avoid copying, or rely on compiler optimizations like RVO/NRVO (Return Value Optimization/Named Return Value Optimization).

#### 1. RVO/NRVO
RVO (Return Value Optimization) and NRVO (Named Return Value Optimization) are compiler optimization techniques that allow the compiler to construct return objects directly in the caller's memory space, thereby avoiding unnecessary copy operations. However, this optimization is not mandatory; the compiler can choose not to apply it.

```cpp
std::string createString() {
    std::string str = "Hello, World!";
    return str; // Compiler may apply RVO/NRVO optimization
}
```

#### 2. Pass by reference
Another common approach is to pass object references to avoid copying:

```cpp
void processString(const std::string& str) {
    // Process string
}
```
While this method avoids copying, it cannot enable resource transfer and still has performance bottlenecks.

#### 3. Using output parameters / in-place construction to avoid intermediate objects
Sometimes, developers use output parameters or in-place construction to avoid creating intermediate objects:

```cpp
void createString(std::string& outStr) {
    outStr = "Hello, World!"; // Construct directly on output parameter
}
```
While this method is effective, it increases code complexity and is not intuitive.

### 4. Ownership transfer with pointers/handles: std::auto_ptr (historical), raw pointers, smart pointers (Boost)
Before C++11, managing dynamically allocated resources (like memory) typically relied on raw pointers or smart pointers (like Boost's `shared_ptr` and `scoped_ptr`). However, these methods did not directly solve the problem of object ownership transfer.

`std::auto_ptr` was a smart pointer introduced in C++98 designed to manage dynamically allocated resources. It implemented ownership transfer by overloading the copy constructor and assignment operator. However, `std::auto_ptr` had some issues, such as causing unexpected behavior when used in containers. Considering everything, it was deprecated in C++11 and replaced by `std::unique_ptr`.

```cpp
std::auto_ptr<MyClass> ptr1(new MyClass());
std::auto_ptr<MyClass> ptr2 = ptr1; // Ownership transfer
```

> `std::auto_ptr` was not move semantics, but rather a historical compromise where "the copy interface was forced to bear the responsibility of ownership transfer semantics." This is also one of the direct motivations for C++11 to explicitly distinguish copy from move.

#### 5. Copy-on-Write (COW) / Reference-counted shared representation
Another technique commonly used before C++11 was Copy-on-Write (COW) and reference-counted shared representation. This technique allows multiple objects to share the same data, only performing a copy when the data needs to be modified, thereby reducing unnecessary copy operations.

```cpp
class String {
public:
    // Reference-counted shared representation
};
```
Although COW and reference counting can improve performance, they increase code complexity and may introduce data race issues in multithreaded environments.

### When to use T& / const T&, and when to use T&&?
- Use `T&` or `const T&` when you want to pass a persistent object and don't need to modify it.
- Use `T&&` when you want to pass a temporary object and want to be able to "steal" its resources.

Here are some specific examples:

#### 1. If the function parameter is an object and you want to be able to modify it, use `T&`

> The implication of lvalue reference is "I'm referencing an object that will continue to be used, and my modifications to it will affect its subsequent use."

```cpp
void foo(T& obj) {
    // Modify obj
}
foo(existingObject); // Pass a persistent object
foo(T()); // Error, cannot pass a temporary object
```

#### 2. If the function parameter is an object and you don't want to modify it, use `const T&`

> The conventional way to pass objects, suitable for most scenarios.

```cpp
void foo(const T& obj) {
    // Read-only access to obj
}
foo(existingObject); // Pass a persistent object
foo(T()); // Pass a temporary object
```

#### 3. As mentioned above: If the function parameter is an object and you want to be able to "steal" its resources, use `T&&`

> The implication of rvalue reference is "I'm referencing an object that is about to be destroyed, and I can steal its resources because it won't be used again."
> Of course, "about to be destroyed" is not quite accurate. A more precise way to put it is "the object's task is complete, and it no longer bears responsibility."

```cpp
void foo(T&& obj) {
    // Steal obj's resources
}
foo(T()); // Pass a temporary object, the most natural usage
foo(existingObject); // Error, cannot pass a persistent object
foo(std::move(existingObject)); // Correct, convert to rvalue reference via std::move, for named objects using rvalue reference
```

### Summary

- `T&`: I borrow your thing (you'll use it later), I might modify it
- `const T&`: I'm just looking (you'll use it later), I won't modify it
- `T&&`: You're ready to give it up, so I'll take the usable resources (consume)

By understanding the essence and use cases of rvalue references, developers can more effectively manage object lifetimes and resources, thereby improving program performance and maintainability. I hope this helps everyone better understand this important feature introduced in C++11.
