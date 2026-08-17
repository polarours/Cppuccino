# Template Specialization Pitfalls

## Introduction

Template specialization lets you provide a different implementation for particular types. It is powerful, but it has sharp edges:

- **Function templates cannot be *partially* specialized** — only fully specialized. Programmers who try the obvious syntax get a hard compile error.
- **Specializing a `std` template** (other than a handful of user-customizable traits) is **undefined behavior**.
- A specialization must be declared in the **same namespace** as the primary template and after it is visible, or it silently fails to be selected.

Each of these produces either a build break or, worse, code that compiles but never uses your specialization.

## The Problem

### 1. Partial specialization of a function template (does not compile)

```cpp
#include <iostream>

template <typename T>
void print(const T& v) { std::cout << v; }

// ERROR: function templates may not be partially specialized
template <typename T>
void print<T*>(const T* v) { std::cout << "ptr:" << *v; }
```

### 2. Specializing a standard library container (undefined behavior)

```cpp
#include <vector>
struct MyType { int x; };

namespace std {
    // UB: you may not specialize std containers like vector for your type
    template <> class vector<MyType> { /* ... */ };
}
```

**Why it's broken:**
- The compiler rejects `print<T*>` because the language simply forbids partial specialization of function templates; use overloading instead.
- Specializing `std::vector` for `MyType` is UB: the standard only permits specializing a few traits (`std::hash`, `std::numeric_limits`, `std::less`, etc.) and only when the specialization depends on a user-defined type. Doing otherwise yields unpredictable behavior and breaks ABI assumptions.

## The Fix

### 1. Replace function partial specialization with overloading

```cpp
#include <iostream>

template <typename T>
void print(const T& v) { std::cout << v; }

template <typename T>
void print(const T* v) { std::cout << "ptr:" << *v; }   // overload, NOT specialization
```

Overload resolution picks the pointer version for pointer arguments and the generic one otherwise — exactly the effect you wanted, and it is legal.

### 2. Customize via traits / your own types, never by specializing `std` containers

```cpp
#include <vector>
struct MyType { int x; };

// Give MyType its own container or a trait; do NOT reopen namespace std.
template <typename Alloc = std::allocator<MyType>>
using MyVec = std::vector<MyType, Alloc>;   // alias, not a specialization
```

If you need custom behavior for a user-defined type, define your own function/type or a `std`-permitted trait (e.g. specialize `std::hash<MyType>`), never a container.

## Best Practices

- **For "special-case a function for some types", write overloads**, not partial specializations. Full specializations of function templates are allowed but usually unnecessary given overloading.
- **Never partially specialize function templates.** It is ill-formed; the fix is always an overload.
- **Do not specialize `std` templates** except the explicitly allowed ones (`std::hash`, `std::numeric_limits`, `std::less`, comparison traits). When in doubt, put your logic in a trait of your own.
- **Declare specializations in the same namespace, after the primary template is visible.** A specialization the compiler hasn't seen yet is simply ignored — a silent, maddening bug.
- **Keep class-template specializations minimal and intentional.** Prefer tag dispatch or `if constexpr` over a sprawl of specializations when the logic is closely related.

## Summary

Specialization is easy to reach for and easy to misuse. The two traps that bite most often are attempting partial specialization of a function template (use an overload) and trying to specialize a standard container (UB — use a trait or your own type). Get those right and specialization becomes a precise tool instead of a source of build breaks and silent mis-selection.
