# Rule of Zero, Rule of Three, and Rule of Five

## 1. Introduction

In C++, special member functions are not incidental boilerplate.
They define ownership transfer, copy behavior, destruction policy,
and therefore the safety boundaries of a type.

When these semantics are implicit but incorrect,
code can compile cleanly and still fail catastrophically at runtime.

The Rule of Zero, Rule of Three, and Rule of Five are not competing slogans.
They are decision frameworks for one core question:

> does this type directly own resources, and if yes, how are copy/move/destruction supposed to behave?

---

## 2. Scope and Non-Goals

### 2.1 Scope

This chapter covers:

1. what each rule means and where it applies,
2. how compiler-generated special members are affected by user declarations,
3. ownership models and their mapping to copy/move policy,
4. practical patterns for robust type design.

### 2.2 Non-Goals

We do not list every edge case of implicit generation wording.
The focus is production-grade reasoning and maintainable engineering choices.

---

## 3. The Six Special Member Functions

In modern C++, a class may have these special members:

1. default constructor,
2. destructor,
3. copy constructor,
4. copy assignment operator,
5. move constructor,
6. move assignment operator.

Their implicit declaration/definition depends on what you declare explicitly.
One custom declaration can alter generation of others.

This dependency graph is the reason the rules exist.

---

## 4. Rule of Three, Historically and Still Relevant

### 4.1 Statement

If a class defines one of:

- destructor,
- copy constructor,
- copy assignment,

it likely needs all three.

### 4.2 Why

These three are tightly coupled in resource-owning classes.
If one is customized, default behavior of others is often incorrect.

Classic failure: destructor frees memory, default copy performs shallow pointer copy.

---

## 5. Rule of Five in C++11+

With move semantics, ownership transfer can be efficient and explicit.

Rule of Five extends Rule of Three:

if you need custom copy/destruction logic,
you probably also need custom:

- move constructor,
- move assignment.

Otherwise, type may become accidentally non-movable,
or movable with undesirable semantics.

---

## 6. Rule of Zero as Preferred Default

Rule of Zero says:

> if your type does not directly manage a raw resource,
> do not define special members manually.

Instead, compose from well-behaved RAII members:

- `std::vector`,
- `std::string`,
- `std::unique_ptr`,
- other value-semantic types.

Then compiler-generated operations are usually correct by construction.

---

## 7. Why This Matters: Minimal Dangerous Example

```cpp
class Buffer {
public:
    explicit Buffer(std::size_t n)
        : n_(n), data_(new char[n]{}) {}

    ~Buffer() { delete[] data_; }

private:
    std::size_t n_{};
    char* data_{};
};
```

This class owns memory and defines destructor.
But copy constructor and copy assignment are implicitly generated.

Result:

1. shallow copy of `data_`,
2. two objects think they own same allocation,
3. double deletion on destruction (undefined behavior).

---

## 8. Correcting with Rule of Five

One valid approach is fully explicit ownership policy:

```cpp
class Buffer {
public:
    explicit Buffer(std::size_t n)
        : n_(n), data_(new char[n]{}) {}

    ~Buffer() { delete[] data_; }

    Buffer(const Buffer& other)
        : n_(other.n_), data_(new char[other.n_]) {
        std::memcpy(data_, other.data_, n_);
    }

    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;
        char* tmp = new char[other.n_];
        std::memcpy(tmp, other.data_, other.n_);
        delete[] data_;
        data_ = tmp;
        n_ = other.n_;
        return *this;
    }

    Buffer(Buffer&& other) noexcept
        : n_(other.n_), data_(other.data_) {
        other.n_ = 0;
        other.data_ = nullptr;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) return *this;
        delete[] data_;
        n_ = other.n_;
        data_ = other.data_;
        other.n_ = 0;
        other.data_ = nullptr;
        return *this;
    }

private:
    std::size_t n_{};
    char* data_{};
};
```

This is correct but verbose and error-prone.

---

## 9. Preferred Fix: Rule of Zero by Composition

```cpp
#include <vector>

class Buffer {
public:
    explicit Buffer(std::size_t n) : data_(n) {}

private:
    std::vector<char> data_;
};
```

Now copy/move/destruction semantics are delegated to `std::vector`.
Most manual pitfalls disappear.

---

## 10. Ownership Taxonomy and Rule Choice

### 10.1 Non-Owning View Types

Examples: `std::string_view`, raw observer pointer, span-like types.
Usually Rule of Zero applies.

### 10.2 Unique Ownership Types

Examples: file descriptor wrapper, socket handle owner, unique buffer owner.
Rule of Five may apply unless ownership is delegated to `std::unique_ptr` and friends.

### 10.3 Shared Ownership Types

Examples: shared object graphs.
Prefer standard shared primitives and explicit graph design.

---

## 11. Implicit Generation: Practical Heuristics

Detailed standard wording is complex,
but these engineering heuristics are reliable:

1. user-declared destructor often changes move generation expectations,
2. user-declared copy members may suppress implicit move operations,
3. declaring one special member should trigger full policy review.

Never assume compiler defaults stay ideal after class evolution.

---

## 12. `=default` and `=delete` as Design Tools

Use `=default` when default behavior is correct and should stay explicit.

Use `=delete` to prohibit invalid operations.

Example (move-only type):

```cpp
class Handle {
public:
    Handle() = default;
    ~Handle() = default;

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    Handle(Handle&&) noexcept = default;
    Handle& operator=(Handle&&) noexcept = default;
};
```

This is clearer than relying on accidental non-copyability.

---

## 13. Exception Safety and Assignment Design

Copy assignment should often provide strong guarantee.
Common pattern:

1. build new state first,
2. swap/commit after success.

The rules are not only about ownership correctness,
but also about predictable failure behavior.

---

## 14. `noexcept` and Move Operations

Mark move operations `noexcept` when truly non-throwing.
Many generic components and containers use this trait for strategy selection.

Incorrect `noexcept` is dangerous,
but missing `noexcept` can degrade behavior and performance characteristics.

---

## 15. Rule of Zero and API Clarity

Rule of Zero improves:

1. correctness,
2. readability,
3. maintainability under refactoring.

Because ownership intent is expressed through member types,
not through fragile handcrafted special-member code.

---

## 16. Common Anti-Patterns

1. raw owning pointer + custom destructor + no copy/move policy.
2. deleting copy but forgetting to define move for movable resource owner.
3. defaulting move despite members that make moved-from state invalid.
4. mixing owning and non-owning raw pointers without clear naming/contracts.

All four are frequent sources of latent defects.

---

## 17. Legacy Code Modernization Strategy

For older C++ codebases:

1. identify classes with raw owning pointers,
2. classify ownership model,
3. migrate to standard owners where possible,
4. enforce explicit `=default` / `=delete` contracts,
5. add tests for copy/move/destruction behavior.

Do this incrementally to reduce regression risk.

---

## 18. Value Semantics vs Resource Semantics

Not every type should be copyable.
Copyability is an API contract, not a default entitlement.

Design questions:

1. does copying make semantic sense?
2. is deep copy affordable and expected?
3. should type be move-only?

Answering these early prevents interface churn later.

---

## 19. Testing Checklist for Special Members

For resource-owning classes, test:

1. copy construction correctness and independence,
2. copy assignment self-assignment safety,
3. move construction leaves source valid,
4. move assignment releases prior resource safely,
5. destruction after moved-from operations is safe.

Unit tests should include exceptional paths where relevant.

---

## 20. Code Review Checklist

Before merging class changes, verify:

- [ ] Does class directly own any resource?
- [ ] Is chosen rule (Zero/Three/Five) explicit and justified?
- [ ] Are copy/move operations aligned with ownership policy?
- [ ] Are `noexcept` annotations correct for move operations?
- [ ] Are raw pointers ownership-annotated or replaced?
- [ ] Does refactoring preserve prior semantic contracts?

If ownership is unclear in review, implementation is not ready.

---

## 21. Rule Selection Decision Matrix

Practical matrix:

1. no direct ownership -> Rule of Zero,
2. direct ownership with manual policy -> Rule of Five,
3. legacy pre-C++11 context -> Rule of Three framing,
4. API should be non-copyable -> delete copy, define/allow move explicitly.

Use this matrix as design gate at class introduction time.

---

## 22. Conclusion

Rule of Zero, Rule of Three, and Rule of Five describe one continuum:

1. prefer composition and compiler-generated correctness,
2. customize special members only when ownership semantics require it,
3. make copy/move/destruction contracts explicit and testable.

In modern C++, the best default is Rule of Zero.
When you must leave it, do so deliberately and completely.
Partial customization is where most ownership bugs begin.
