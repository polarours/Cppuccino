# Moving from a `const` Object

## 1. Problem Statement

Many C++ developers write `std::move(x)` and assume move semantics automatically happen.
That assumption is incomplete.

`std::move` does not move anything by itself.
It only casts an expression to an rvalue category.

When the source object is `const`,
that cast usually cannot call useful move operations,
so the program falls back to copy semantics.

This pitfall creates two problems:

1. performance illusions (code looks optimized but is not),
2. semantic confusion in API design.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This article covers:

1. why `std::move(const T&)` commonly copies,
2. overload-resolution mechanics behind that behavior,
3. practical API design patterns for ownership transfer,
4. review checks to catch false-move code.

### 2.2 Non-Goals

We do not claim no type can move from `const`.
Some custom types could provide `const T&&` overloads,
but this is rare and usually undesirable.

---

## 3. Minimal Example

```cpp
#include <string>
#include <utility>

void f(const std::string& s) {
    std::string x = std::move(s); // copy, not move
}
```

Why?

`std::move(s)` has type `const std::string&&`.
`std::string` move constructor expects `std::string&&` (non-const rvalue).
That cannot bind to const source for move-stealing semantics.

So copy constructor is selected instead.

---

## 4. Core Language Mechanics

### 4.1 `std::move` Is a Cast Utility

Conceptually:

```cpp
template <class T>
constexpr std::remove_reference_t<T>&& move(T&& t) noexcept;
```

No memory transfer occurs inside `std::move`.
Only value category conversion.

### 4.2 cv-Qualification Persists

`const` qualification is preserved.

If input is `const T&`,
result is `const T&&`, not `T&&`.

### 4.3 Move Operations Usually Mutate Source

Most move constructors/assignments need to modify source object
(e.g., nulling pointer, resetting size).
Modifying `const` source is forbidden.

Hence typical move overload takes `T&&`, not `const T&&`.

---

## 5. Overload Resolution Outcome

Common constructor set:

```cpp
T(const T&); // copy
T(T&&);      // move
```

Given `const T&&` argument:

1. `T(T&&)` is not viable (cannot drop const),
2. `T(const T&)` is viable,
3. copy constructor wins.

So writing `std::move(const_obj)` often means “explicitly request copy-like path.”

---

## 6. Why This Bug Is So Common

### 6.1 Visual Heuristic Error

Developers equate “contains `std::move`” with “is fast move path.”

### 6.2 API Signature Mismatch

Functions taking `const T&` cannot transfer ownership from caller.
Yet maintainers add `std::move` internally hoping for optimization.

### 6.3 Silent Behavior

Compiler usually accepts code with no warning.
Behavioral difference is semantic/performance, not syntactic.

---

## 7. Demonstration with Instrumented Type

```cpp
#include <iostream>
#include <utility>

struct X {
    X() = default;
    X(const X&) { std::cout << "copy\n"; }
    X(X&&) noexcept { std::cout << "move\n"; }
};

void g(const X& x) {
    X y = std::move(x); // prints copy
}
```

This often surprises people who expected `move`.

---

## 8. False Optimization Anti-Pattern

Pattern:

```cpp
void set_name(const std::string& s) {
    name_ = std::move(s); // still copy assignment in most implementations
}
```

This code suggests optimization intent but provides none.
It can also mislead future maintainers.

---

## 9. Correct API Pattern: Pass by Value + Move

For sink-like APIs:

```cpp
void set_name(std::string s) {
    name_ = std::move(s);
}
```

Callers passing rvalues can move into parameter.
Callers passing lvalues copy once into parameter.

This pattern often gives clean semantics with good performance.

---

## 10. Overload Pair Pattern

Alternative explicit form:

```cpp
void set_name(const std::string& s) { name_ = s; }
void set_name(std::string&& s) noexcept { name_ = std::move(s); }
```

Use when distinct behavior/cost modeling is desired.
Pass-by-value version is frequently simpler and adequate.

---

## 11. Perfect Forwarding Context

In forwarding templates:

```cpp
template <class T>
void sink(T&& x) {
    consume(std::forward<T>(x));
}
```

If caller passes `const T&`, forwarding preserves constness,
and downstream move still cannot steal from const source.

Forwarding does not bypass const-correctness.

---

## 12. `const` Return Values and Move Suppression

Returning `const T` by value is usually harmful:

1. it may inhibit move in assignment contexts,
2. it rarely provides meaningful safety,
3. it complicates optimization expectations.

Prefer returning plain `T` by value.

---

## 13. Moved-From State and Const

Move operations rely on valid-but-unspecified moved-from state transitions.
Those transitions are state mutations.

`const` prohibits that mutation.
This is why “move from const” generally contradicts move semantics intent.

---

## 14. Can a Type Support `const T&&` Move?

Technically possible to define overloads taking `const T&&`,
but usually they cannot perform resource stealing.

Such designs are uncommon and can confuse users.
Most codebases should avoid relying on this niche behavior.

---

## 15. Containers and Const Elements

For associative containers,
elements may expose const key parts.
Attempting to move const key subobjects still cannot steal.

Understand container element constness before assuming move optimizations.

---

## 16. Concurrency and Const Misconception

`const` does not imply thread safety,
and inability to move from const does not imply immutability guarantees for shared data races.

Do not mix const-correctness discussion with synchronization guarantees.

---

## 17. Diagnostic Strategies

To detect false-move patterns:

1. inspect function signatures (`const T&` + `std::move` is suspicious),
2. use compiler warnings/linters for pessimizing or ineffective moves,
3. profile copy/move counts in benchmarks for critical paths.

Static review is usually enough for many cases.

---

## 18. Common Anti-Patterns

1. `std::move` on `const` local variable.
2. `std::move` on function parameter declared `const T&`.
3. returning `const T` and expecting move-friendly behavior.
4. adding `const` to temporaries “for safety” in performance-critical paths.
5. assuming forwarding always enables move.

---

## 19. Safer Design Rules

1. If ownership transfer is intended, do not accept `const T&`.
2. Prefer pass-by-value sink APIs for simple ownership transfer interfaces.
3. Use `std::move` only on non-const objects when moving is semantically valid.
4. Keep constness where logical immutability is needed, not as default decoration.
5. Document ownership semantics in API contracts.

---

## 20. Migration Guidance

For existing code:

1. grep for `std::move(` inside functions taking `const T&`.
2. classify those call sites into sink vs borrow APIs.
3. convert sink APIs to pass-by-value or rvalue overloads.
4. keep borrow APIs copy-based and explicit.
5. re-benchmark critical paths after signature updates.

---

## 21. Code Review Checklist

- [ ] Is `std::move` applied to a `const` object?
- [ ] Does function signature match intended ownership transfer?
- [ ] Are sink APIs modeled as value/rvalue interfaces?
- [ ] Are copy paths explicit where borrowing is intended?
- [ ] Is performance assumption validated rather than inferred from syntax?

If any answer is uncertain, redesign API contract first.

---

## 22. Practical Summary

1. `std::move` is a cast, not a move operation.
2. `std::move(const T&)` usually leads to copy behavior.
3. Ownership-transfer APIs should avoid `const` input contracts.
4. Pass-by-value + internal move is a robust default sink pattern.
5. Optimize with measured data, not with symbolic `std::move` usage.

---

## 23. Conclusion

“Move from const” is one of the most persistent C++ performance misconceptions.
It survives because syntax looks right while semantics differ.

The durable engineering mindset is:

> design API signatures for ownership intent first,
> then apply `std::move` where non-const move semantics are actually available.

When constness and ownership contracts are aligned,
code becomes both clearer and faster in ways that are real, not cosmetic.
