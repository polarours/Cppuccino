# Lvalue and Rvalue References: Binding Rules

## The Foundation of the C++ Reference System

In C++, references are a powerful concept, and understanding lvalue and rvalue
references is the key to mastering modern C++.

## Basic Concepts

### Lvalue

An lvalue is an expression denoting a persistent object or function. It
outlives the expression that mentions it.

```cpp
int x = 42;           // x is an lvalue
int& ref = x;         // an lvalue reference binds to an lvalue
```

Test: **it has a name, and you can take its address.**

### Rvalue

An rvalue is a temporary or soon-to-be-destroyed object. It has no persistent
identity.

```cpp
int x = 42;
int y = x + 1;        // the result of x + 1 is an rvalue
int&& rref = 42;      // an rvalue reference binds to an rvalue
```

Test: **no name, temporary, movable.**

## Reference Types

### 1. Lvalue reference (`T&`)

```cpp
int a = 10;
int& b = a;    // OK: binds to an lvalue
// int& c = 10;  // error: a non-const lvalue reference cannot bind to an rvalue
```

### 2. Const lvalue reference (`const T&`)

```cpp
const int& d = 10;  // OK: a const lvalue reference can bind to an rvalue
int e = 10;
const int& f = e;   // OK: binds to an lvalue
```

### 3. Rvalue reference (`T&&`)

```cpp
#include <utility>

int x = 10;
int&& g = 10;       // OK: binds to an rvalue
// int&& h = x;     // error: an rvalue reference cannot bind to an lvalue
int&& i = std::move(x);  // OK: explicit conversion to an rvalue
```

## Key Rules

### Rule 1: Binding matrix

| Type | What it can bind to |
|------|---------------------|
| `T&` | lvalues only |
| `const T&` | lvalues and rvalues |
| `T&&` | rvalues only |

### Rule 2: Reference collapsing

During template deduction, the reference folding rules apply:

- `T& &` → `T&`
- `T& &&` → `T&`
- `T&& &` → `T&`
- `T&& &&` → `T&&`

```cpp
template <typename T>
void perfectForward(T&& arg) {  // T&& is a forwarding reference
    // If an lvalue is passed, T deduces to T&, and T&& folds to T&
    // If an rvalue is passed, T deduces to T, and T&& stays T&&
}
```

### Rule 3: Lifetime extension of temporaries

```cpp
const int& ref = 42;  // the temporary lives as long as ref
```

Only a **const lvalue reference** (or an rvalue reference) bound directly to a
temporary extends its lifetime.

## Practical Uses

### 1. Move semantics

```cpp
#include <vector>
#include <utility>

void demo() {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = std::move(v1);  // transfer ownership, no copy
}
```

### 2. Perfect forwarding

```cpp
#include <utility>

template <typename T>
void otherFunction(T&&) {}

template <typename T>
void wrapper(T&& arg) {
    otherFunction(std::forward<T>(arg));  // preserves the original value category
}
```

### 3. Avoiding unnecessary copies

```cpp
#include <string>

void process(const std::string& str);  // read-only access, no copy
void process(std::string str);         // takes ownership, by value
```

## Common Traps

### Trap 1: Dangling reference

```cpp
const int& getRef();               // returns a reference (not a value)

const int& ref = getRef();  // Dangerous! If getRef() referred to a
                            // temporary, that temporary is already gone.
                            // Binding a reference to a function call result
                            // never extends any lifetime.
```

Note: `const int& r = f();` where `f()` returns an `int` **by value** is safe —
`r` binds to the prvalue and extends its lifetime (Rule 3). The danger starts
when a *reference* comes back.

### Trap 2: Misusing an rvalue reference

```cpp
void foo(int&& x) {
    // x is an rvalue reference, but inside the function body it is an lvalue!
    int& ref = x;  // OK: x is an lvalue here
}
```

### Trap 3: Reference bound to a deleted object

```cpp
int* ptr = new int(42);
int& ref = *ptr;
delete ptr;
// ref dangles now!
```

## Example

A runnable version of this topic:

- [reference-binding-rules-demo.cpp](../../examples/reference-binding-rules-demo.cpp)

## Summary

- Lvalue reference (`T&`): binds lvalues — expresses "borrowing"
- Const lvalue reference (`const T&`): binds lvalues and rvalues — expresses
  "read-only access"
- Rvalue reference (`T&&`): binds rvalues — expresses "movable"
- Understanding the binding rules is the foundation of move semantics and
  perfect forwarding
- Watch out for dangling references and rvalue-reference misuse

> **Core idea: a reference is an alias, not an object. Choosing the right
> reference type states your intent in the type system.**
