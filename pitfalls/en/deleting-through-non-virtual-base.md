# Deleting Through a Non-Virtual Base Pointer

## 1. Problem Statement

In C++, deleting through a base-class pointer is safe only if the base has a virtual destructor.
If the base destructor is non-virtual,
deleting a derived object via base pointer causes undefined behavior.

This pitfall is severe because code often compiles cleanly,
may appear to run correctly in simple tests,
and then fails unpredictably in production.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This article covers:

1. why non-virtual-base deletion is UB,
2. object-model reasoning for destructor dispatch,
3. common failure manifestations,
4. robust class design rules and alternatives,
5. review and migration checklists.

### 2.2 Non-Goals

We do not discourage inheritance broadly.
The target is safe polymorphic lifetime management.

---

## 3. Minimal Failing Example

```cpp
struct Base {
    ~Base() = default; // non-virtual
};

struct Derived : Base {
    ~Derived() { /* release resources */ }
};

Base* p = new Derived;
delete p; // undefined behavior
```

The delete expression uses static type `Base*`.
Without virtual destructor, dynamic destruction of `Derived` is not guaranteed.

---

## 4. Why This Is Undefined Behavior

Polymorphic destruction requires runtime dispatch of destructor chain.

With non-virtual base destructor:

1. delete expression does not perform required dynamic destructor dispatch,
2. derived subobject cleanup may be skipped,
3. deallocation function interaction may become inconsistent,
4. program enters undefined behavior territory.

This is not merely “resource leak risk”; it is a formal UB condition.

---

## 5. Object Model View

For a `Derived` object seen through `Base*`:

1. static type at call site is `Base*`,
2. dynamic type at runtime is `Derived`.

Correct deletion requires respecting dynamic type.
Virtual destructor in base provides that dynamic dispatch path.

No virtual destructor means static-type destruction logic is used,
which is insufficient for polymorphic lifetime teardown.

---

## 6. Typical Symptoms in Real Code

Although UB has no guaranteed symptom, common outcomes include:

1. leaked resources from skipped derived destructor,
2. partial object teardown leading to invariant violations,
3. heap corruption under custom allocators,
4. crashes only under optimization or specific allocator behavior.

Because symptoms are nondeterministic, this bug is expensive to debug.

---

## 7. Correct Baseline Rule

If a class is intended to be a polymorphic base,
declare virtual destructor explicitly:

```cpp
struct Base {
    virtual ~Base() = default;
};
```

This single line encodes deletion safety contract for derived objects.

---

## 8. If Polymorphism Is Not Intended

If base is not meant for polymorphic use,
you can prevent accidental inheritance/deletion misuse by:

1. marking class `final`,
2. making destructor protected/private where appropriate,
3. avoiding virtual functions entirely.

Design intent should be explicit.

---

## 9. Anti-Pattern: “One Virtual Method but Non-Virtual Destructor”

A class with other virtual methods but non-virtual destructor is a high-risk smell.

Reason:

1. API invites polymorphic usage,
2. deletion path remains unsafe.

For polymorphic interfaces,
virtual destructor is part of minimum contract.

---

## 10. Interface-Only Base Classes

Abstract interface pattern:

```cpp
struct ITask {
    virtual ~ITask() = default;
    virtual void run() = 0;
};
```

Even if no data members exist,
virtual destructor is still required for safe polymorphic deletion.

---

## 11. Smart Pointers Do Not Magically Fix This

`std::unique_ptr<Base>` deleting `Derived` object through default deleter
still relies on base destructor semantics.

If base destructor is non-virtual,
`unique_ptr<Base>` can trigger the same UB as raw pointer delete.

Smart pointers improve ownership management,
but cannot repair invalid class destruction contracts.

---

## 12. Custom Deleter Workaround (Use Sparingly)

A custom deleter can technically avoid base-delete UB:

```cpp
std::unique_ptr<Base, void(*)(Base*)> p(
    new Derived,
    [](Base* b) { delete static_cast<Derived*>(b); }
);
```

This is fragile:

1. type coupling leaks into owner site,
2. unsafe if dynamic type assumption changes,
3. not scalable for open polymorphic hierarchies.

Prefer fixing base contract instead.

---

## 13. Protected Virtual Destructor Pattern

Some frameworks disallow heap deletion through public API.
Pattern:

```cpp
struct Base {
protected:
    virtual ~Base() = default;
};
```

This still keeps polymorphic destruction semantics,
while restricting who can delete.

Use only when ownership model is carefully designed.

---

## 14. Construction/Destruction Dispatch Clarification

Virtual dispatch behaves specially during construction/destruction.
But that rule does not negate need for virtual destructor.

Two separate facts:

1. ctor/dtor virtual calls do not dispatch to most-derived override,
2. delete through base pointer requires virtual base destructor for safe teardown.

Do not conflate them.

---

## 15. ABI and Library Evolution Perspective

For public libraries,
adding virtual destructor later can be ABI-affecting.

Therefore, decide base-class polymorphic contract early.
Late correction can be costly for binary compatibility and downstream users.

---

## 16. Performance Concern Myth

Concern: virtual destructor adds overhead.

Reality:

1. overhead is usually negligible compared to correctness risk,
2. polymorphic hierarchy already pays virtual dispatch cost,
3. wrong optimization target leads to unsafe APIs.

Safety contract should dominate micro-cost speculation.

---

## 17. Related Pitfall: Slicing + Non-Virtual Deletion

Projects with by-value base APIs often also have deletion-contract issues.
Both stem from misunderstanding static vs dynamic type roles.

If hierarchy is polymorphic:

1. avoid slicing interfaces,
2. enforce virtual base destructor,
3. use explicit ownership types.

---

## 18. Migration Strategy for Legacy Code

1. Identify classes used polymorphically.
2. Audit base destructors.
3. Add `virtual ~Base() = default;` where required.
4. Re-run tests with sanitizers and stress paths.
5. Review ABI implications for distributed binaries.

For non-polymorphic bases,
consider sealing hierarchy or removing accidental virtual usage.

---

## 19. Static Analysis and Tooling

Use compiler warnings and linters that flag:

1. virtual methods with non-virtual destructor,
2. unsafe deletion patterns,
3. suspicious base-pointer ownership conversions.

Tooling catches many cases early,
but design review remains essential.

---

## 20. Code Review Checklist

- [ ] Is class intended for polymorphic use?
- [ ] If yes, does base have virtual destructor?
- [ ] Are objects ever deleted through base pointer/reference-owned wrappers?
- [ ] Are smart-pointer aliases using correct base deletion contract?
- [ ] Is ownership model explicit (who deletes and how)?
- [ ] Are ABI implications considered for public libraries?

If any answer is unclear, class contract is incomplete.

---

## 21. Design Guidance Summary

1. Polymorphic base -> virtual destructor, always.
2. Non-polymorphic base -> avoid exposing polymorphic deletion paths.
3. Smart pointers require correct base contract; they are not a substitute.
4. Treat destructor design as API-level ownership contract.
5. Validate with warnings, analysis tools, and explicit code review.

---

## 22. Conclusion

Deleting through a non-virtual base pointer is not a minor style defect.
It is a formal undefined behavior pitfall rooted in object-model contract violation.

The reliable rule is simple and should be institutionalized:

> if a class can be used polymorphically and deleted through base type,
> base destructor must be virtual.

Applying this rule consistently eliminates an entire class of severe lifetime bugs.
