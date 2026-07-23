# Virtual Dispatch and the C++ Object Model

## 1. Introduction

Runtime polymorphism in C++ is often introduced as “virtual functions + inheritance.”
That description is syntactically true but semantically incomplete.

Correct reasoning requires three dimensions at the same time:

1. static type at call site,
2. dynamic type of the object,
3. lifetime phase of that object.

Most severe polymorphism bugs happen when one of these dimensions is ignored.

This chapter focuses on practical rigor:

> understand when dynamic dispatch happens, when it intentionally does not happen, and how object lifetime constrains all polymorphic operations.

---

## 2. Scope and Non-Goals

### 2.1 Scope

We cover:

- virtual dispatch mechanics in language terms,
- construction/destruction dispatch rules,
- object slicing and destruction safety,
- cast behavior in polymorphic hierarchies,
- engineering design and review checklists.

### 2.2 Non-Goals

We do not reverse-engineer ABI internals of every platform.
The chapter is language- and design-focused.

---

## 3. Core Terms

### 3.1 Static Type

Type known at compile time from declaration.

```cpp
Base* p = ...; // static type of p is Base*
```

### 3.2 Dynamic Type

Most-derived type of the object currently referenced at runtime.

If `p` points to `Derived`, dynamic type is `Derived`.

### 3.3 Polymorphic Type

A class with at least one virtual function.
Only polymorphic types support runtime type features like `dynamic_cast` to derived/reference forms and `typeid` dynamic behavior.

---

## 4. Minimal Dispatch Model

Basic example:

```cpp
#include <iostream>

struct Base {
    virtual ~Base() = default;
    virtual void f() { std::cout << "Base\n"; }
};

struct Derived : Base {
    void f() override { std::cout << "Derived\n"; }
};
```

Calling `f()` through `Base*` or `Base&` to a `Derived` object dispatches to `Derived::f()`.

---

## 5. What Is Actually Required for Dynamic Dispatch

Dynamic dispatch requires all of:

1. call expression is virtual-call eligible,
2. call is through base reference/pointer (or equivalent context),
3. object is alive and polymorphic state is valid for that phase.

If you call by value on sliced object,
or call non-virtual member,
dispatch behavior is entirely different.

---

## 6. Construction and Destruction Rule (Critical)

During construction or destruction of a class subobject,
virtual calls do **not** dispatch to more-derived overrides.

This rule exists to prevent access to not-yet-constructed or already-destroyed derived state.

Consequence:

1. base constructor virtual call targets base implementation,
2. base destructor virtual call targets base implementation.

Treat virtual calls in ctors/dtors as design smell unless intentionally scoped.

---

## 7. Demonstration of Ctor/Dtor Dispatch Behavior

```cpp
#include <iostream>

struct Base {
    Base() { f(); }
    virtual ~Base() { f(); }

    virtual void f() { std::cout << "Base::f\n"; }
};

struct Derived : Base {
    void f() override { std::cout << "Derived::f\n"; }
};
```

Constructing `Derived` invokes `Base::f()` inside `Base` constructor.
Destroying `Derived` eventually invokes `Base::f()` inside `Base` destructor.

This is defined behavior, not optimization artifact.

---

## 8. Virtual Destructor: Non-Negotiable in Polymorphic Bases

If objects may be deleted through base pointer,
base destructor must be virtual.

Incorrect pattern:

```cpp
struct Base {
    ~Base() = default; // non-virtual
};

struct Derived : Base {
    ~Derived() { /* release resources */ }
};
```

`delete Base*` pointing to `Derived` is undefined behavior.

Correct pattern:

```cpp
struct Base {
    virtual ~Base() = default;
};
```

---

## 9. Object Slicing and Lost Polymorphism

Polymorphism requires indirection (reference/pointer), not by-value base objects.

```cpp
void log(Base b); // slicing risk
```

Passing `Derived` by value into `Base` parameter slices derived part.
Dynamic dispatch expectation is invalid after slicing.

API design rule:

1. use `Base&` / `Base*` for polymorphic interfaces,
2. use value semantics only for non-polymorphic value objects.

---

## 10. Override Discipline

Use `override` on every intended override.

Benefits:

1. catches signature mismatch at compile time,
2. protects against accidental non-overrides due to const/ref/noexcept differences,
3. improves review readability.

Avoid silent shadowing.

---

## 11. `final` as Intent and Optimization Hint

Use `final` where extension is intentionally prohibited:

```cpp
struct Leaf final : Base {
    void f() override;
};
```

This is primarily design communication.
Potential devirtualization benefits are secondary and toolchain-dependent.

---

## 12. Covariant Return Types

C++ supports covariant returns for overridden virtual functions returning pointers/references to classes.

```cpp
struct Base { virtual Base* clone() const = 0; };
struct Derived : Base { Derived* clone() const override; };
```

Useful for polymorphic cloning patterns,
but ownership semantics must still be explicit (prefer smart pointers in modern APIs).

---

## 13. RTTI, `dynamic_cast`, and `typeid`

`dynamic_cast` to downcast across polymorphic hierarchy can be safe when checked.

Guideline:

1. prefer virtual interface design over frequent downcasts,
2. when downcast is unavoidable, use checked `dynamic_cast` not `static_cast` guesswork.

`typeid` on polymorphic glvalue gives dynamic type information.
Without polymorphism, behavior differs.

---

## 14. Multiple Inheritance and Complexity

Multiple inheritance can be valid,
but dispatch and layout reasoning become more complex.

Risks increase around:

1. ambiguous base member names,
2. pointer adjustments,
3. diamond inheritance without clear virtual inheritance strategy.

Prefer interface-focused inheritance or composition when feasible.

---

## 15. Non-Virtual Interface (NVI) Pattern

NVI pattern helps control invariants:

```cpp
struct Base {
    void run() { pre(); do_run(); post(); }
    virtual ~Base() = default;

private:
    virtual void do_run() = 0;
    void pre();
    void post();
};
```

External contract remains non-virtual,
while customization point is isolated.

---

## 16. Pure Virtual Functions and Base Destructors

A class with pure virtuals can still provide concrete destructor body.
If destructor is declared pure virtual, definition is still required.

```cpp
struct Abstract {
    virtual ~Abstract() = 0;
};

inline Abstract::~Abstract() = default;
```

This is a common point of confusion.

---

## 17. Lifetime Safety and Polymorphic Calls

Virtual dispatch does not rescue use-after-free bugs.
If object lifetime has ended,
any member call is invalid regardless of virtual semantics.

Use ownership-safe handles (`std::unique_ptr`, `std::shared_ptr` with clear policy)
and avoid raw owning pointers.

---

## 18. Concurrency and Polymorphism

Virtual dispatch itself is not synchronization.
Calling virtual methods concurrently on same object requires proper thread-safety design.

Document:

1. whether methods are thread-safe,
2. who owns synchronization,
3. whether dynamic object mutation can race with destruction.

---

## 19. Performance Notes (Without Myths)

Virtual call cost is usually one indirect call plus branch prediction effects.
In many applications, algorithm/data-structure choices dominate that cost.

Do not remove polymorphism prematurely.
Measure first.

Potential optimization options:

1. `final` where semantically valid,
2. devirtualization opportunities via whole-program analysis,
3. alternative polymorphism models (CRTP, variants) when profile-driven.

---

## 20. Common Anti-Patterns

1. Polymorphic base without virtual destructor.
2. Virtual calls from ctors/dtors expecting derived behavior.
3. Passing polymorphic objects by value.
4. Downcasting via `static_cast` without proof.
5. Exposing mutable base references without ownership/lifetime contract.

---

## 21. Design Decision Matrix

Choose runtime polymorphism when:

1. behavior set is open for extension,
2. dispatch must be decided at runtime,
3. object identity and lifetime are central.

Prefer alternatives (templates/variants) when:

1. type set is closed,
2. compile-time dispatch is acceptable,
3. performance profile justifies complexity shift.

---

## 22. Review Checklist

Before merging polymorphic hierarchy changes:

- [ ] Base intended for polymorphic deletion has virtual destructor.
- [ ] All overrides marked with `override`.
- [ ] No ctor/dtor virtual call relies on derived state.
- [ ] APIs avoid slicing by using references/pointers.
- [ ] Casting operations are checked and justified.
- [ ] Lifetime ownership is explicit and testable.

If any item is unclear, hierarchy is not production-ready.

---

## 23. Practical Guidance Summary

1. Design polymorphism around explicit contracts.
2. Keep base interfaces minimal and stable.
3. Use `override`/`final` deliberately.
4. Treat lifetime rules as first-class constraints.
5. Favor clarity over inheritance cleverness.

---

## 24. Conclusion

Virtual dispatch in C++ is safe and predictable when three axes are reasoned together:

1. static type,
2. dynamic type,
3. lifetime phase.

Most bugs come from violating this triad, not from virtual dispatch itself.

A disciplined object-model mindset—virtual destructor correctness,
constructor/destructor dispatch awareness,
anti-slicing API design,
and explicit ownership—turns polymorphism into a robust tool rather than a source of hidden defects.

## See Also

- **Related docs:** [Ownership and Lifetime](ownership-and-lifetime.md), [The True Semantics of `const`](the-true-semantics-of-const.md), [PImpl and ABI Stability](pimpl-and-abi-stability.md)
- **Pitfalls:** [Object Slicing](../pitfalls/en/object-slicing.md), [Deleting Through Non-Virtual Base](../pitfalls/en/deleting-through-non-virtual-base.md), [Template Deduction Pitfalls](../pitfalls/en/template-deduction-pitfalls.md)
- **Examples:** [virtual-dispatch-example.cpp](../examples/virtual-dispatch-example.cpp), [virtual-dispatch-and-slicing.cpp](../examples/virtual-dispatch-and-slicing.cpp), [crtp-basics.cpp](../examples/crtp-basics.cpp)
