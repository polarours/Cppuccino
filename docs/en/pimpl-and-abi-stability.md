# PImpl and ABI Stability

## 1. Introduction

In C++, binary compatibility is fragile because type layout and symbol-level details are part of ABI.
If private data members are exposed in public headers,
seemingly harmless internal refactoring can force client recompilation or even break runtime compatibility.

PImpl (Pointer to Implementation), also called the Cheshire Cat idiom,
is a deliberate boundary technique:

> keep public interface stable while allowing private implementation to evolve.

This chapter explains not only how to write PImpl,
but when it is worth the cost and how to avoid common correctness traps.

---

## 2. Scope and Non-Goals

### 2.1 Scope

We cover:

1. ABI sensitivity in ordinary class design,
2. canonical PImpl structure,
3. special member function implications,
4. performance and allocation trade-offs,
5. practical decision framework for production code.

### 2.2 Non-Goals

We do not attempt to document all ABI rules for every compiler/platform pair.
The focus is engineering principles that hold broadly.

---

## 3. Why ABI Breaks So Easily in C++

### 3.1 Layout and Inline Coupling

Changing private members can affect:

- class size,
- alignment,
- inlined function code generation,
- exception unwinding metadata,
- symbol usage patterns.

Even if source-level API appears unchanged,
binary compatibility can still be broken.

### 3.2 Header Exposure Multiplies Dependencies

When headers include heavy implementation types,
every downstream translation unit sees that dependency graph.

Consequences:

1. large rebuild impact,
2. fragile include order interactions,
3. harder modular boundaries.

---

## 4. Core Idea of PImpl

Public class stores a pointer to opaque implementation type.
Header exposes only:

- stable interface,
- forward declaration of `Impl`,
- pointer member.

All concrete representation details move to `.cpp`.

---

## 5. Canonical Skeleton

```cpp
// widget.h
#pragma once
#include <memory>

class Widget {
public:
    Widget();
    ~Widget();

    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;

    Widget(const Widget&);
    Widget& operator=(const Widget&);

    void draw();

private:
    struct Impl;
    std::unique_ptr<Impl> p_;
};
```

`Impl` definition resides in `.cpp`:

```cpp
// widget.cpp
#include "widget.h"

struct Widget::Impl {
    // private fields and helper methods
};
```

---

## 6. Why Destructor Is Usually Out-of-Line

With `std::unique_ptr<Impl>`, deletion requires complete `Impl` type at destruction site.
If destructor is implicitly generated inline in header,
the compiler may need full `Impl` where clients include header.

Therefore, common pattern is:

1. declare `~Widget();` in header,
2. define `Widget::~Widget() = default;` in `.cpp` after `Impl` is complete.

This preserves opacity and avoids incomplete-type pitfalls.

---

## 7. Move Semantics with PImpl

Move operations are often straightforward:

- moving `std::unique_ptr` transfers ownership,
- moved-from object remains valid with null/empty impl pointer.

Example:

```cpp
Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;
```

Marking move as `noexcept` is typically correct for unique ownership pointer transfer.

---

## 8. Copy Semantics with PImpl

Copying cannot be defaulted naively if deep copy is required,
because `std::unique_ptr` is non-copyable.

Common deep-copy pattern:

```cpp
Widget::Widget(const Widget& other)
    : p_(other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr) {}

Widget& Widget::operator=(const Widget& other) {
    if (this == &other) return *this;
    p_ = other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr;
    return *this;
}
```

Decide copy policy explicitly:

1. deep copy,
2. shared state,
3. non-copyable API.

---

## 9. Exception Safety Considerations

Copy assignment via allocation should preserve strong guarantee where possible.

Safer variant uses copy-and-swap style:

```cpp
Widget& Widget::operator=(const Widget& other) {
    if (this == &other) return *this;
    auto tmp = other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr;
    p_.swap(tmp);
    return *this;
}
```

Allocate first, then swap, so failure leaves original unchanged.

---

## 10. Const-Correct Interface Design

Public `const` member functions should preserve logical constness.
Even though `Impl` is hidden, const promises still apply.

If mutable caches exist inside `Impl`,
document thread-safety and visible behavior carefully.

---

## 11. ABI Stability Benefits in Practice

PImpl helps stabilize:

1. class size and member layout visible to clients,
2. recompilation surface for internal changes,
3. dependency propagation from private headers.

For library maintainers, this can significantly reduce release risk.

---

## 12. Build-Time Benefits

By moving private includes to `.cpp`,
header parse cost and transitive dependencies can drop substantially.

Large codebases often see:

- smaller rebuild sets,
- cleaner layering,
- reduced accidental coupling.

---

## 13. Runtime Costs and Their Meaning

PImpl introduces trade-offs:

1. extra indirection on member access,
2. dynamic allocation for `Impl`,
3. potential cache locality loss,
4. more boilerplate and code volume.

These costs may be negligible for coarse-grained APIs,
but can be material in hot micro-paths.

---

## 14. Small Buffer and Custom Allocation Variants

For performance-sensitive contexts, teams may consider:

1. custom allocator for `Impl`,
2. arena allocation,
3. small-buffer optimization (SBO-like PImpl).

These variants increase complexity and ABI design burden.
Use only when measurement justifies.

---

## 15. Inlining Trade-Off

Because implementation is hidden in `.cpp`,
cross-translation-unit inlining opportunities can decrease (subject to LTO availability).

If API requires heavy tiny-call performance,
evaluate whether opaque boundary is acceptable.

---

## 16. Interaction with Modules and LTO

C++ Modules reduce parse-time cost and macro leakage,
but do not remove ABI concerns for binary-distributed libraries.

LTO can recover some optimization opportunities,
yet does not negate the semantic boundary role of PImpl.

PImpl and modern toolchains are complementary, not mutually exclusive.

---

## 17. Symbol Visibility and ABI Policy

PImpl is one component of ABI strategy.
Robust libraries also manage:

1. symbol visibility (`-fvisibility=hidden` style policies),
2. exported API set discipline,
3. versioning and deprecation process.

Without policy, PImpl alone cannot guarantee compatibility governance.

---

## 18. Common Pitfalls

1. Defining destructor inline and triggering incomplete-type issues.
2. Forgetting to define copy semantics policy explicitly.
3. Assuming PImpl automatically provides thread safety.
4. Using PImpl for tiny value types where cost dominates benefit.
5. Leaking `Impl` details back into header through inline helpers.

---

## 19. When PImpl Is a Good Fit

Use PImpl when most of these are true:

1. library has long binary compatibility horizon,
2. internals are expected to evolve frequently,
3. rebuild cost is significant,
4. one-pointer indirection cost is acceptable.

---

## 20. When PImpl Is Not Ideal

Avoid or reconsider when:

1. type is performance-critical in tight loops,
2. value-semantics small-object layout is key API feature,
3. project is monolithic and always rebuilt from source,
4. ABI compatibility is not a requirement.

---

## 21. Worked Example with Complete Special Members

```cpp
// widget.h
#pragma once
#include <memory>

class Widget {
public:
    Widget();
    ~Widget();

    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;

    Widget(const Widget&);
    Widget& operator=(const Widget&);

    void draw() const;

private:
    struct Impl;
    std::unique_ptr<Impl> p_;
};
```

```cpp
// widget.cpp
#include "widget.h"
#include <utility>

struct Widget::Impl {
    int width = 0;
    int height = 0;
    void draw() const {
        // render details
    }
};

Widget::Widget() : p_(std::make_unique<Impl>()) {}
Widget::~Widget() = default;

Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;

Widget::Widget(const Widget& other)
    : p_(other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr) {}

Widget& Widget::operator=(const Widget& other) {
    if (this == &other) return *this;
    auto tmp = other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr;
    p_.swap(tmp);
    return *this;
}

void Widget::draw() const {
    p_->draw();
}
```

This pattern keeps ABI-facing layout stable while preserving value-like behavior.

---

## 22. Review Checklist

Before adopting or modifying PImpl classes, verify:

- [ ] Is ABI stability actually a requirement?
- [ ] Destructor is out-of-line where needed.
- [ ] Copy/move policy is explicit and tested.
- [ ] Exception safety level is documented.
- [ ] Header does not leak implementation-only dependencies.
- [ ] Runtime overhead is measured, not assumed.

---

## 23. Migration Strategy for Existing Types

For legacy classes:

1. freeze public API shape,
2. introduce opaque `Impl` and pointer member,
3. move private data/functions to `.cpp`,
4. re-establish copy/move semantics,
5. run ABI and performance regression checks.

Migration is manageable when done incrementally with tests.

---

## 24. Conclusion

PImpl is a strategic engineering trade-off, not a default style rule.

It offers:

1. stronger ABI resilience,
2. reduced compile-time coupling,
3. better encapsulation boundaries.

It costs:

1. indirection,
2. allocation,
3. additional code complexity.

Adopt it when compatibility horizon and encapsulation needs justify that cost.
When chosen deliberately and implemented carefully,
PImpl is one of the most effective tools for long-lived C++ library design.
