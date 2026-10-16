# C++20 Concepts In-Depth (C++20 Concepts)

## What Are Concepts?

Concepts are C++20's compile-time constraint system: you **explicitly
declare** what a template parameter must support, instead of implicitly
relying on runtime accidents like "this type happens to have `operator+`".

> **Core idea**: turn "what capabilities this type must have" from a comment
> into a contract the compiler checks.

## Basic Concept Syntax

```cpp
#include <concepts>
#include <iostream>

// custom concept: printable types
template <typename T>
concept Printable = requires(T a) {
    { std::cout << a } -> std::convertible_to<std::ostream&>;
};

// custom concept: addable types
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

// composed concept
template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;
```

- a `requires` clause lists the requirements
- `{ expr } -> Concept` asserts that the *result* satisfies a concept
- `std::convertible_to<A, B>` / `std::same_as<A, B>` come from the standard
  library

## Concepts on Function Templates

```cpp
#include <concepts>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

// only types satisfying Number may call this
template <Number T>
T add(T a, T b) {
    return a + b;
}

auto x = add(1, 2);       // OK
// auto y = add("a", "b"); // would fail: const char* does not satisfy Number
// (the diagnostic reads: "constraints not satisfied for 'add' ... Number"
//  — compile the line above uncommented to see it)
```

Without a concept:

```cpp
// no constraint: the error only appears at instantiation, and it is opaque
template <typename T>
T add(T a, T b) { return a + b; }
```

## Concepts on Class Templates

```cpp
#include <vector>
#include <ranges>
#include <utility>

template <std::ranges::range R>
class ContainerWrapper {
public:
    explicit ContainerWrapper(R r) : data_(std::move(r)) {}

    auto begin() { return data_.begin(); }
    auto end()   { return data_.end(); }

private:
    R data_;
};
```

`std::ranges::range` requires the type to have `begin()` and `end()` — the
compiler guarantees any `R` used with this class template is iterable.

## Constrained Aliases

```cpp
#include <concepts>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
using NumberRef = T&;

int value = 42;
NumberRef<int> x = value;  // same as int&, but constrained
```

## Compound Requirements and Tracing Return Types with `->`

```cpp
#include <concepts>
#include <cstddef>

template <typename T>
concept ContainerLike = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};
```

- a `requires` clause may contain *compound requirements*:
  `{ expr } -> Concept;`
- these check that `expr` is valid **and** that its result satisfies the
  concept

## Two Forms of `requires`

```cpp
#include <concepts>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

// form 1: requires clause (after the template parameter list)
template <Number T>
T add(T a, T b);

// form 2: requires expression (inside a concept definition)
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};
```

It also works on member functions:

```cpp
#include <concepts>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

class Calculator {
public:
    template <Number T>
    T calculate(T a, T b) requires(true) {
        return a + b;
    }
};
```

## Standard Library Concepts Cheat Sheet

| Concept | Requires |
|---|---|
| `std::integral<T>` | an integral type |
| `std::floating_point<T>` | a floating-point type |
| `std::same_as<A, B>` | A and B are the same type |
| `std::convertible_to<A, B>` | A implicitly converts to B |
| `std::copyable<T>` | copy-constructible, copy-assignable, destructible |
| `std::movable<T>` | move-constructible, move-assignable, destructible |
| `std::ranges::range<T>` | has `begin()`/`end()` |
| `std::random_access_range<T>` | supports `[]`, `<`, `-` |

## Testing Concept Satisfiability

```cpp
#include <concepts>
#include <ranges>
#include <vector>

static_assert(std::integral<int>);
static_assert(!std::integral<double>);
static_assert(std::ranges::range<std::vector<int>>);
```

This verifies at compile time that a concept is defined the way you think.

## Best Practices

1. **Define concepts in headers**: they are part of the template declaration
   and must be visible
2. **Name concretely**: `HasToString` beats `Stringable`; `Addable` is more
   precise than `Numeric`
3. **Prefer `requires` clauses over SFINAE**: friendlier diagnostics and better
   tooling support
4. **Keep concepts precise and small**: express `IntegralOrFloat` as
   `std::integral || std::floating_point` instead of hand-rolling long lists
5. **Don't over-constrain**: `requires std::size_t` reads better than
   `requires T == std::size_t`

## Summary

Concepts are C++ template programming's step from "guess the problem from
compiler errors" to "state the contract with constraints". They turn
`template<typename T>` into `template<Number T>`, so errors degrade from
mysterious deduction failures into clear diagnostics like "int does not
satisfy the concept Number".
