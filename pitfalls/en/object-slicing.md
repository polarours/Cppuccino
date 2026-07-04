# Object Slicing

## 1. Problem Statement

Object slicing occurs when a derived object is copied or assigned into a base object **by value**.
Only the base subobject is preserved; derived-state is discarded.

This is not an optimization detail.
It is a direct consequence of value semantics in C++ object model.

Because slicing compiles cleanly and often runs “normally,”
it is a high-risk correctness pitfall in polymorphic codebases.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This article covers:

1. why slicing happens,
2. where it appears in real code,
3. semantic and architectural consequences,
4. robust API and design alternatives,
5. review/migration checklists.

### 2.2 Non-Goals

We do not claim inheritance should be avoided entirely.
The goal is to avoid value-semantic misuse in polymorphic hierarchies.

---

## 3. Minimal Example

```cpp
struct Base {
    virtual ~Base() = default;
    virtual const char* name() const { return "Base"; }
};

struct Derived : Base {
    const char* name() const override { return "Derived"; }
};

void print(Base b) { // slicing
    // b is a standalone Base object
}
```

Passing a `Derived` object into `print` creates a new `Base` object.
Derived part is gone in `b`.

---

## 4. Why It Happens (Object Model View)

`Derived` contains a `Base` subobject plus additional state/behavior context.
By-value parameter `Base b` requires constructing a concrete `Base` object.

That construction copies only the base subobject.
No dynamic type relationship is preserved in the copied object.

Polymorphism in C++ requires indirection (`Base&` / `Base*`),
not by-value base copies.

---

## 5. Static vs Dynamic Type Clarification

Before slicing:

1. static type at call site may be `Base&` or `Base*`,
2. dynamic type may be `Derived`.

After slicing into `Base` value:

1. static type is `Base`,
2. dynamic type is also `Base`.

Expecting derived virtual behavior after slicing is logically inconsistent.

---

## 6. Typical Manifestations

### 6.1 By-Value Parameters

```cpp
void log(Base b);
```

### 6.2 By-Value Return Through Base

```cpp
Base make_base_like(); // may hide slicing in implementation
```

### 6.3 Container of Base Values

```cpp
std::vector<Base> xs;
xs.push_back(Derived{}); // slices at insertion
```

### 6.4 Assignment to Base Object

```cpp
Base b;
Derived d;
b = d; // derived state lost
```

---

## 7. Semantic Consequences

Slicing can cause:

1. loss of derived invariants/state,
2. unexpected virtual dispatch outcomes,
3. silently wrong business behavior,
4. difficult-to-diagnose logic divergence.

Even without crashes, correctness can be compromised.

---

## 8. “Virtual Functions Don’t Save Sliced Objects”

Virtual dispatch relies on dynamic type of **the object being called on**.
After slicing, that object is a genuine `Base`.

So calling virtual functions on sliced `Base` value correctly dispatches to base implementations.
This is expected behavior, not compiler bug.

---

## 9. Safe Interface Pattern: References/Pointers

For polymorphic APIs, use:

1. `const Base&` for non-owning read access,
2. `Base&` for mutable non-owning access,
3. `Base*` / smart pointers for nullable or owning paths.

Example:

```cpp
void print(const Base& b) {
    // dynamic dispatch preserved
}
```

---

## 10. Container Design: Avoid `std::vector<Base>` for Polymorphism

If elements are polymorphic, prefer owning indirection:

```cpp
std::vector<std::unique_ptr<Base>> items;
items.push_back(std::make_unique<Derived>());
```

This preserves dynamic type and avoids slicing.

---

## 11. Ownership and Lifetime Considerations

Avoiding slicing often introduces pointer/reference usage.
That shifts focus to ownership clarity.

Recommended:

1. use `std::unique_ptr<Base>` for unique ownership,
2. use `std::shared_ptr<Base>` only for genuine shared lifetime,
3. keep raw pointers as non-owning observers.

---

## 12. Copy Semantics in Polymorphic Bases

Polymorphic bases copied by value are suspicious by default.
Many designs should prohibit base copying:

```cpp
struct Base {
    Base() = default;
    Base(const Base&) = delete;
    Base& operator=(const Base&) = delete;
    virtual ~Base() = default;
};
```

This blocks accidental slicing through copy paths.

---

## 13. Polymorphic Cloning Pattern

When deep-copy-like behavior is needed, use virtual clone interface:

```cpp
#include <memory>

struct Base {
    virtual ~Base() = default;
    virtual std::unique_ptr<Base> clone() const = 0;
};

struct Derived : Base {
    std::unique_ptr<Base> clone() const override {
        return std::make_unique<Derived>(*this);
    }
};
```

This preserves dynamic type without slicing.

---

## 14. Slicing in Constructors and Factories

Beware factory signatures returning `Base` by value.
Even if implementation constructs `Derived`,
result type forces slicing.

Prefer:

1. `std::unique_ptr<Base>` return,
2. or references managed by clear owner.

---

## 15. Hidden Slicing in Utility Functions

Common subtle case:

```cpp
template <class T>
void consume_base(Base b, T&& x);
```

Template forwarding around this can hide slicing at call sites.
APIs should avoid by-value base parameters entirely in polymorphic paths.

---

## 16. Logging/Debug Code Also Needs Discipline

Slicing often enters code via “harmless” logging helpers:

```cpp
void debug_dump(Base b);
```

Such helpers may become correctness-critical over time.
Use reference-based signatures consistently.

---

## 17. Performance Myths

Some developers pass base by value to “avoid pointer indirection.”
In polymorphic design this is usually incorrect trade-off,
because it destroys semantic intent.

Performance optimization should not invalidate object-model contracts.

---

## 18. Tooling and Diagnostics

Enable warnings/analysis that detect slicing opportunities.

Useful checks:

1. pass-by-value of polymorphic base,
2. insertion of derived into base-valued containers,
3. suspicious copy constructors in polymorphic hierarchies.

Tooling helps, but design review is primary defense.

---

## 19. Common Anti-Patterns

1. Polymorphic API accepting `Base` by value.
2. Returning `Base` by value from virtual/factory abstractions.
3. Storing polymorphic objects in `std::vector<Base>`.
4. Assuming virtual dispatch still reaches derived after slicing.
5. Mixing value-semantics assumptions with runtime-polymorphism goals.

---

## 20. Migration Strategy for Existing Code

1. Search for by-value parameters/returns of polymorphic bases.
2. Replace with references/pointers/smart pointers based on ownership.
3. Refactor base-value containers to owning polymorphic containers.
4. Add tests verifying derived behavior survives through API layers.
5. Consider deleting copy operations on polymorphic bases.

---

## 21. Review Checklist

- [ ] Is any polymorphic base passed/returned by value?
- [ ] Are polymorphic objects stored in base-value containers?
- [ ] Is ownership explicit where references/pointers are introduced?
- [ ] Are clone/copy semantics explicit for polymorphic duplication?
- [ ] Are warnings for slicing enabled in CI/tooling?
- [ ] Do tests assert dynamic behavior, not just static compilation?

If any answer is uncertain, slicing risk remains.

---

## 22. Safer Form Summary

1. Use `const Base&` / `Base&` / `Base*` for polymorphic APIs.
2. Use smart pointers for ownership-aware polymorphic storage/returns.
3. Avoid base-by-value interfaces in polymorphic designs.
4. Explicitly design cloning when polymorphic copy is required.
5. Treat slicing as semantic bug, not cosmetic issue.

---

## 23. Conclusion

Object slicing is a direct consequence of value semantics applied to inheritance hierarchies.
It is predictable, silent, and frequently harmful.

The reliable engineering rule is simple:

> runtime polymorphism requires indirection;
> base-by-value APIs are incompatible with preserving derived behavior.

When teams enforce this consistently,
an entire class of subtle polymorphism bugs can be eliminated early in review.
