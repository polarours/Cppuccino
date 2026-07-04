# `noexcept` and Move Operations

## 1. Introduction

`noexcept` in C++ is not merely documentation.
It is an executable contract that affects:

1. what optimizations generic code can safely apply,
2. which strategy standard containers choose,
3. whether exception-safety guarantees can be preserved.

For move operations, this becomes especially important.
A type with potentially throwing move often causes containers to prefer copy during reallocation,
which can change both complexity constants and behavior expectations.

This chapter focuses on one engineering objective:

> understanding when and why move operations should be marked `noexcept`, and how to do so without lying to the type system.

---

## 2. Scope and Non-Goals

### 2.1 Scope

We cover:

- language semantics of `noexcept`,
- interaction with move constructor/assignment,
- container strategy selection (`move-if-noexcept` style behavior),
- conditional `noexcept` patterns,
- code review and API design guidelines.

### 2.2 Non-Goals

We do not attempt to benchmark all STL implementations.
The objective is semantic correctness and portable reasoning.

---

## 3. What `noexcept` Means Semantically

### 3.1 Contract Meaning

A function declared `noexcept` promises that exception propagation will not leave the function.
If an exception escapes, program calls `std::terminate`.

Therefore, `noexcept` is stronger than a comment.
It changes both program behavior and optimizer assumptions.

### 3.2 `noexcept` as Type Property in Generic Contexts

`noexcept` participates in compile-time reasoning:

```cpp
static_assert(noexcept(std::declval<int&>() = 1));
```

Templates can branch on `std::is_nothrow_move_constructible_v<T>` and related traits.

---

## 4. Why Move + `noexcept` Matters to Containers

Consider `std::vector` growth:

1. allocate new storage,
2. transfer old elements,
3. deallocate old storage.

To keep strong exception guarantee, element transfer must not leave container corrupted.
If move may throw, many implementations choose copy when copy is safer.

Hence:

- nothrow move often enables preferred relocation path,
- potentially throwing move can trigger fallback behavior.

---

## 5. Minimal Example and Consequence

```cpp
class Node {
public:
    Node() = default;
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;

    Node(const Node&) = default;
    Node& operator=(const Node&) = default;
};
```

This declaration informs generic code that moving `Node` is non-throwing.
Container internals can safely prefer move in many transfer scenarios.

---

## 6. When You Must Not Mark Move `noexcept`

Declaring `noexcept` without proof is a contract violation risk.

If a subobject move can throw, unconditional `noexcept` can make failure terminate program unexpectedly.

Example risk pattern:

```cpp
class Risky {
public:
    Risky(Risky&&) noexcept { // dangerous if members may throw
        // ... moving subobjects that may throw
    }
};
```

Engineering rule:

> mark nothrow only when transitive move path is demonstrably non-throwing.

---

## 7. Conditional `noexcept`: Preferred Pattern

For composed types, use conditional specification:

```cpp
template <class Member>
class Wrapper {
public:
    Wrapper() = default;

    Wrapper(Wrapper&&)
        noexcept(std::is_nothrow_move_constructible_v<Member>) = default;

    Wrapper& operator=(Wrapper&&)
        noexcept(std::is_nothrow_move_assignable_v<Member>) = default;

private:
    Member m_;
};
```

This keeps contract aligned with member capabilities.

---

## 8. Move Constructor vs Move Assignment

These two operations have different failure surfaces.

1. move constructor builds a new object from source,
2. move assignment mutates an existing object, often after releasing old state.

A type may legitimately be nothrow-move-constructible but not nothrow-move-assignable.
Do not assume symmetry.

---

## 9. Rule of Zero and `noexcept`

If class delegates ownership to standard members (`std::vector`, `std::unique_ptr`, etc.),
Rule of Zero often gives correct move semantics with proper noexcept propagation automatically.

Manual special-member definitions should be used when semantics truly require customization,
not by default habit.

---

## 10. Interaction with `std::move_if_noexcept`

`std::move_if_noexcept(x)` returns:

- `T&&` if move construction is known nothrow (or copy unavailable),
- otherwise `const T&` to favor copy path.

This utility embodies the same policy many generic components use internally.

```cpp
template <class T>
void relocate(T& x) {
    T y(std::move_if_noexcept(x));
}
```

Correct `noexcept` on your type directly affects this behavior.

---

## 11. Strong Exception Guarantee Perspective

For containers and generic algorithms,
strong guarantee usually means operation either:

1. succeeds completely,
2. or leaves observable state unchanged.

Potentially throwing move can make rollback logic complex or impossible in cheap form.
Nothrow move is therefore a structural enabler, not just micro-optimization.

---

## 12. RAII and Failure Boundaries

Even with `noexcept` move, class invariants must remain valid on all paths.
Design principles:

1. moved-from objects remain valid (though unspecified state),
2. ownership transfer is single and explicit,
3. no leaks under partial transfer logic.

`noexcept` does not repair bad ownership design.

---

## 13. Practical Type Categories

### 13.1 Usually Nothrow-Movable

- types wrapping raw handles with simple swap/steal logic,
- smart pointers like `std::unique_ptr`,
- aggregates of nothrow-movable members.

### 13.2 Potentially Throwing Move

- types that allocate during move,
- types with user-defined invariants requiring fallible operations,
- wrappers over third-party components with throwing move.

Classify honestly before annotating.

---

## 14. Common Anti-Patterns

1. Marking move `noexcept` to “speed up vector” without proof.
2. Forgetting conditional `noexcept` in templated wrappers.
3. Assuming copy/move noexcept status remains unchanged after member evolution.
4. Equating “rarely throws” with “cannot throw.”

Each anti-pattern weakens reliability of generic code.

---

## 15. Evolution Hazard in Real Codebases

A type may start nothrow-movable,
then later add a member whose move can throw.

If old unconditional `noexcept` remains,
contract silently becomes invalid.

Mitigation:

1. prefer defaulted special members with conditional noexcept,
2. add static assertions on key traits,
3. include noexcept traits in API compatibility checks.

---

## 16. Testing Strategy

You cannot test all exception paths by chance.
Use layered checks:

1. compile-time traits (`is_nothrow_move_constructible`),
2. targeted tests for moved-from validity,
3. sanitizer-assisted leak/dangling checks,
4. stress tests on container reallocation-heavy workloads.

---

## 17. Diagnostics Helpers

Simple diagnostic assertions:

```cpp
template <class T>
constexpr void require_nothrow_move() {
    static_assert(std::is_nothrow_move_constructible_v<T>,
                  "T must be nothrow-move-constructible");
}
```

Use carefully in performance-critical generic components where policy requires this property.

---

## 18. API Design Guidance

When exposing reusable types:

1. document move/copy exception guarantees,
2. keep moved-from state contract explicit,
3. avoid surprising throwing behavior in fundamental operations,
4. align implementation with declared noexcept contract.

Consumers build assumptions based on these traits.

---

## 19. Review Checklist

Before merging type changes, verify:

- [ ] Move constructor noexcept status is justified.
- [ ] Move assignment noexcept status is justified separately.
- [ ] Conditional noexcept reflects member traits.
- [ ] No unconditional noexcept hides fallible sub-operations.
- [ ] Trait-based behavior changes are documented.
- [ ] Container performance/correctness impact is evaluated.

If any item is unclear, prefer conservative contracts.

---

## 20. Version Notes

- C++11 introduced move semantics and `noexcept`.
- Modern standard library implementations heavily use noexcept traits in dispatch decisions.

Exact internal heuristics are implementation details,
but the broad contract-driven behavior is stable enough for engineering design.

---

## 21. Worked Comparison

```cpp
struct A {
    A(A&&) noexcept = default;
    A(const A&) = default;
};

struct B {
    B(B&&) = default;   // potentially throwing
    B(const B&) = default;
};
```

In generic relocation contexts,
`A` is more likely to follow move path,
while `B` may trigger copy-preserving strategy.

This difference can appear even when source code “looks similar.”

---

## 22. Conclusion

`noexcept` on move operations is part of your type’s semantic ABI for generic code.
It influences both safety strategy and performance behavior.

Reliable engineering practice is:

1. declare `noexcept` only when proven,
2. use conditional `noexcept` for composed/templated types,
3. treat trait changes as API-significant events,
4. optimize after contracts are correct.

In short, `noexcept` is not decoration;
it is a correctness and optimization boundary that should be designed deliberately.
