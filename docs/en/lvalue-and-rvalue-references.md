# C++ References: Lvalue References (T&) and Rvalue References (T&&)
> **Note**: It's recommended to read the [From Value Categories to Reference Binding](/docs/en/from-value-categories-to-reference-binding.md) chapter before reading this section.
> **Reference**: For detailed definitions of lvalues and rvalues (lvalue / rvalue / prvalue / xvalue), please refer to Section 3.10 "Lvalues and rvalues" in the C++11 standard draft N3337.

## 1. Introduction
C++'s reference mechanism has evolved significantly from lvalue references in C++98 to the introduction of rvalue references in C++11. These two reference types are not in a "advanced vs basic" relationship, but rather **complementary semantic tools** that solve different problems:
- **Lvalue references (T&)** — express "safely continuing to use a persistent object"
- **Rvalue references (T&&)** — express "permission to safely transfer resources because the object is about to be or has already been abandoned"

In one sentence summarizing their design intent:

> If rvalue references solve "how to efficiently abandon and reuse resources," then lvalue references solve "how to safely continue using."

## 2. Lvalue References (T&) — Safe References to Persistent Objects
In early versions of C++, there was no concept of lvalue references. Let's explore the following scenario from that perspective:
```cpp
void process(T value) {
    // Process value
}
```

Problems:
- Every call to `process` copies the object `value`, which is inefficient for large objects.
- If we could directly operate on the original object without copying, performance would improve significantly.

Here we can see that pass-by-value semantics naturally incur a "copy" cost in exchange for safety and independence. The called function can only operate on a copy of the parameter and cannot directly affect the original object in the caller. This isn't problematic when objects are small and copying costs are negligible. However, as programs began heavily using complex data structures (such as dynamic arrays, strings, tree structures, etc.), this "unconditional copying" approach gradually revealed obvious drawbacks:
- High copying costs, especially when involving heap memory allocation;
- Certain types don't want or allow copying;
- Programmers were forced to use pointers to avoid copying, but pointers introduced additional complexity and security risks.

Thus, a natural question arose:

> Is there a mechanism that can be as intuitive as pass-by-value, yet directly operate on the original object like pointers, without introducing pointer-related risks?

Then, we know that C++ solved this problem by introducing **lvalue references (T&)**:

```cpp
void process(T& value) {
    // Directly operate on value, not a copy of it
}
```

Therefore, the purpose of lvalue references is very clear: they allow us to **safely reference and operate on a persistent object** without worrying about the object's lifetime ending before we use it.

## 3. Deep Dive into Lvalue References

### 3.1 Core Semantics of Lvalue References: The Object Will Continue to Be Used
The design intent of lvalue references is to explicitly express that **the current state of this object still has subsequent semantic value**. When using lvalue references, we tell both the compiler and readers:

> This object will remain valid at some future point and can be safely accessed and modified.

For example, consider the following code:
```cpp
#include <iostream>

void updateValue(int& value) {
    std::cout << "[updateValue] value address: " << &value << std::endl;
    value += 10;
}

int main() {
    int x = 5;
    std::cout << "[main] x value: " << x << std::endl;
    std::cout << "[main] x address: " << &x << std::endl;
    updateValue(x);
    std::cout << "[main] x after update: " << x << std::endl;
}
```

Here, `int& value` clearly expresses a reference to `x` and guarantees that `x` is valid during the function call.

### 3.2 Lvalue References and Object Lifetime Management
Lvalue references refer to persistent objects, so we must ensure the object remains valid during the reference's usage period. Incorrect example:
```cpp
int& getReference() {
    int localVar = 1024;
    return localVar; // Error! Returning a reference to a local variable
}
```

In this example, the `getReference` function returns a reference to the local variable `localVar`. However, when the function returns, `localVar`'s lifetime ends, making the reference dangling and causing undefined behavior. Therefore, when using lvalue references, we must ensure the referenced object remains valid while the reference is being used.

Correct approach:
```cpp
int& getReference(int& externalVar) {
    return externalVar; // Correct! Returning a reference to an external variable
}
```

In this modified example, the `getReference` function returns a reference to an external variable whose lifetime is managed by the caller, making it safe.

> For more discussion on undefined behavior, please refer to [Undefined Behavior](/docs/en/undefined-behavior.md).

> Key point: Lvalue references do not extend object lifetimes; they merely provide a safe access pathway, with lifetime still managed by the object itself.

## 4. Rvalue References (T&&) — Allowing Safe Resource Transfer
In contrast to **lvalue references**, **rvalue references** are a reference type introduced in C++11 with the syntax `T&&`. They are used to express a semantic intent: the object no longer bears subsequent responsibility, and its resources can be safely transferred.

To avoid confusion for readers who might not be clear about the concepts of **lvalues** and **rvalues**, let's first briefly review them:
- Lvalues: Originally indicated objects that could be assigned to (i.e., appear on the left side of an assignment operator), characterized by having persistent storage locations and obtainable memory addresses. They mainly include:
    - Variable names or function names (that are in scope)
    - String literals (e.g., "hello", type `const char[N]`, which are immutable lvalues)
    - Dereference operations (e.g., `*ptr`)
- Rvalues: In contrast to lvalues, rvalues represent temporary objects or literals that lack observable object identity and cannot be assigned to. In C++11, the definition became more complex, with rvalues including:
    - Prvalues (pure rvalues): such as literals, temporary objects, etc.
    - Xvalues (expiring values): such as objects converted via `std::move`

Now that we've briefly introduced the concepts of lvalues and rvalues, and we've just explained why C++ needed lvalue references, let's introduce why C++11 later introduced rvalue references. First, let's state the answer:

"Before C++11, the language lacked a safe, standard, and non-copy-semantics-breaking way to express object ownership transfer." We don't say "unable to express object ownership transfer" because imperfect solutions like `std::auto_ptr` existed previously, which will be discussed in detail later.

To explain this statement, let's first consider the following scenario:
```cpp
void push_back(const T& value) {
    data_.push_back(value); // Copy value via copy constructor
}
```

The problem is obvious:
- Every call to `push_back` **copies** the object `value`, which is inefficient for **large objects**.
- If we could **move** the object's resources instead of copying them, we could significantly improve performance.

To solve this problem, C++11 introduced **rvalue references (T&&)**. Before C++11, there was no language-level, universal, and semantically clear mechanism to avoid unnecessary copying; developers could only rely on compiler optimizations or manual techniques, making difficult trade-offs between maintainability and performance.

> Rvalue references allow us to distinguish between **temporary objects** (rvalues) and **persistent objects** (lvalues), thereby enabling **move semantics**.

## 5. Deep Dive into Rvalue References
The syntax for rvalue references is `T&&`, used to express a semantic intent: the object no longer bears subsequent responsibility, and its resources can be safely transferred.

### 5.1 The Essence of the Problem: Can the Object Still Be Used?
In C++, almost all performance issues ultimately come down to object lifetime and ownership management.
Consider a simple example:
```cpp
std::string createString() {
    std::string str = "Hello, World!";
    return str; // Return local variable
}
```

In this example, `str` is a local variable, and when the `createString` function returns, the local variable `str`'s lifetime ends.
Since it can no longer be used, why not directly "move" its resources? This is the core idea behind rvalue references.

### 5.2 Semantics of Rvalue References: Allowing Safe Resource Transfer
Similarly, for lvalue references:
```cpp
void push_back(const std::string& value) {
    data_.push_back(value); // Copy value via copy constructor
}
```

For rvalue references:
```cpp
void push_back(std::string&& value) {
    data_.push_back(std::move(value)); // Transfer value's resources via move constructor
}
```

Therefore, the difference between "lvalues" and "rvalues" can also be explained from this perspective: it's fundamentally about **whether an object's lifetime and ownership can be transferred**.

> It's important to emphasize: whether resources are actually transferred still completely depends on the type's move constructor/move assignment implementation, not the rvalue reference itself.
> For a simple demonstration code, interested readers can refer to [move_semantics_example.cpp](/examples/move_semantics_example.cpp).

### 5.3 How Did C++ Handle This Problem Before Rvalue References?
Before C++11, it wasn't entirely impossible to reduce copying, but there was no mechanism directly expressed by the type system to indicate "transferable ownership," causing all solutions to either rely on optimizations or sacrifice interface design intuitiveness. To optimize performance, developers often had to manually write code to avoid copying or rely on compiler optimizations like RVO/NRVO (Return Value Optimization/Named Return Value Optimization).

#### 5.3.1 RVO/NRVO
**RVO (Return Value Optimization)** and **NRVO (Named Return Value Optimization)** are compiler optimization techniques that allow the compiler to directly construct returned objects in the caller's memory space, avoiding unnecessary copy operations. However, this optimization is not mandatory, and compilers can choose not to perform it.

```cpp
std::string createString() {
    std::string str = "Hello, World!";
    return str; // Compiler might perform RVO/NRVO optimization
}
```

#### 5.3.2 Pass by Reference
Another common approach was passing object references to avoid copying:
```cpp
void processString(const std::string& str) {
    // Process string
}
```

Some readers might wonder: isn't this just lvalue reference? Yes, this is indeed lvalue reference, but it doesn't solve the "resource transfer" problem. While passing by reference avoids copying, if object modification or resource transfer is needed, additional logic is still required.

#### 5.3.3 Output Parameters/In-Place Construction to Avoid Intermediate Objects
Sometimes, developers used output parameters or in-place construction to avoid creating intermediate objects:
```cpp
void createString(std::string& outStr) {
    outStr = "Hello, World!"; // Construct directly on output parameter
}
```

#### 5.3.4 Ownership Transfer via Pointers/Handles: std::auto_ptr (Historical), Raw Pointers, Smart Pointers (Boost)
Before C++11, managing dynamically allocated resources (such as memory) typically relied on raw pointers or smart pointers (like Boost's `shared_ptr` and `scoped_ptr`). However, these methods didn't directly solve the object ownership transfer problem.

##### I. `std::auto_ptr`
`std::auto_ptr` was a smart pointer introduced in C++98 to manage dynamically allocated resources. It implemented ownership transfer by overloading the copy constructor and assignment operator. However, `std::auto_ptr` had some issues, such as causing unexpected behavior when used in containers, so it was deprecated in C++11 and replaced by `std::unique_ptr`.

```cpp
std::auto_ptr<MyClass> ptr1(new MyClass());
std::auto_ptr<MyClass> ptr2 = ptr1; // Ownership transfer
```

> `std::auto_ptr` is not move semantics, but rather a historical compromise where "copy interfaces were forced to bear ownership transfer semantics." This was precisely one of the direct motivations for C++11's explicit distinction between copy/move.

##### II. Raw Pointers: Manual Ownership Transfer
In the era without smart pointers, ownership transfer relied entirely on programmer discipline and documentation. The most common patterns were "factory functions" or "takeover functions."

```cpp
// Pattern A: Transfer ownership via function return value
MyClass* createObject() {
    return new MyClass(); // Caller responsible for delete
}

// Pattern B: Explicit manual transfer
MyClass* ptr1 = new MyClass();
MyClass* ptr2 = ptr1; // Both ptr1 and ptr2 point to it, ownership ambiguous
ptr1 = NULL;          // Programmer manually nulls original pointer, simulating "move"
```

Limitations: This approach easily leads to memory leaks (forgetting delete) or dangling pointers (multiple pointers holding the same address, with one releasing the memory).

##### III. Boost Library: Explicit Ownership Semantics
Before C++11, the Boost library was the de facto standard, using different smart pointers to clarify ownership.

###### boost::scoped_ptr — Strictly Prohibits Transfer
It expresses "absolute ownership that cannot be transferred." It disables copy construction and assignment.

```cpp
#include <boost/scoped_ptr.hpp>

void example() {
    boost::scoped_ptr<MyClass> ptr(new MyClass());
    // boost::scoped_ptr<MyClass> ptr2 = ptr; // Compile error! Transfer or copy not allowed
} // Function ends, ptr automatically releases memory
```

###### boost::shared_ptr — Shared Ownership
It implements "shared ownership" through reference counting. This was the most common solution in C++98 for resolving ownership confusion, although it's not "transfer" but rather "increasing owners."

```cpp
#include <boost/shared_ptr.hpp>

void example() {
    boost::shared_ptr<MyClass> ptr1(new MyClass());
    boost::shared_ptr<MyClass> ptr2 = ptr1; // Reference count increases by 1
} // When ptr1 and ptr2 are destructed, count drops to 0 and memory is actually released
```

#### 5.3.5 Copy-on-Write (COW) / Reference-Counted Shared Representation
Another commonly used technique before C++11 was Copy-on-Write (COW) and reference-counted shared representation. This technique allowed multiple objects to share the same data, only copying when modification was needed, thus reducing unnecessary copying.

The cost of this approach was increased complexity and runtime overhead, along with thread safety issues.

> Specific demonstration code will be provided in [copy_on_write_example.cpp](/examples/copy_on_write_example.cpp).

> None of the above solutions were perfect, having various issues such as complex semantics and performance overhead. C++11's introduction of rvalue references and move semantics was precisely to solve these problems, providing a more intuitive and efficient ownership transfer mechanism.

## 6. Usage Scenarios for T& / const T& / T&&
In C++, understanding when to use lvalue references (`T&`), const lvalue references (`const T&`), and rvalue references (`T&&`) is crucial for writing efficient and semantically clear code. Below are their respective usage scenarios:
- `T&` for borrowing with modification affecting the original object.
- `const T&` for read-only, safe borrowing of persistent or temporary objects.
- `T&&` for transferable ownership, suitable for temporary objects or named objects converted via `std::move`.

Here are some specific examples:

If a function parameter is an object and you want to modify it, use `T&`:
```cpp
void foo(T& obj) {
    // Modify obj
}
foo(existingObject); // Pass a persistent object
foo(T()); // Error, cannot pass temporary object
```

> The implicit message of lvalue references is "I'm referencing an object that will continue to be used, and my modifications will affect its subsequent usage."

If a function parameter is an object and you don't want to modify it, use `const T&`:
```cpp
void foo(const T& obj) {
    // Read-only access to obj
}
foo(existingObject); // Pass a persistent object
foo(T()); // Pass a temporary object
```
> This is the conventional way to pass objects, suitable for most scenarios.

If a function parameter is an object and you want to "safely transfer" its resources, use `T&&`:
```cpp
void foo(T&& obj) {
    // Safely transfer obj's resources
}
foo(T()); // Pass a temporary object, the most natural usage
foo(existingObject); // Error, cannot pass persistent object
foo(std::move(existingObject)); // Correct, convert to rvalue reference via std::move for named objects
```

> Here, `foo` is a non-template function. If `T&&` appears in template parameters, it becomes a forwarding reference, with different behavior.

## 7. Summary
Let's briefly summarize:
- Lvalue references:
  - Lvalue references `T&` are references to persistent objects, allowing efficient operation without copying.
  - They explicitly express that the object will continue to be used in the future, bearing the semantic responsibility of "safe access."
  - Using lvalue references can improve performance, avoid unnecessary copying, and ensure proper object lifetime management.
  - Understanding the design intent and usage scenarios of lvalue references is fundamental to writing efficient and safe C++ code.
- Rvalue references:
  - Rvalue references `T&&` are used to express that an object's resources can be safely transferred because the object no longer bears subsequent responsibility.
  - They allow distinguishing between temporary and persistent objects, enabling move semantics and improving performance.
  - Rvalue references themselves don't guarantee resource transfer; actual move behavior depends on the type's move constructor and move assignment implementation.
  - Understanding the design intent of rvalue references helps better manage object lifetimes and resource ownership.
- Different usage scenarios:
  - `T&`: I'm borrowing your thing (you'll still use it afterward), and I might modify it.
  - `const T&`: I'm just looking (you'll still use it afterward), and I won't modify it.
  - `T&&`: You're ready to abandon it, so I'll take whatever useful resources I can.

By understanding the design intent and usage scenarios of lvalue and rvalue references, developers can more effectively manage object lifetimes and resource ownership, thereby writing efficient and semantically clear C++ code. I hope readers have learned some new knowledge from this chapter to better understand the C++ language. This is also my original intention for writing this documentation.