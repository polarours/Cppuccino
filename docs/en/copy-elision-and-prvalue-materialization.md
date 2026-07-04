# Copy Elision and prvalue Materialization

## 1. Introduction

In modern C++, copy elision is not only a performance topic.
From C++17 onward, parts of “elision behavior” are mandated by core language rules.

Many incorrect explanations still assume a pre-C++17 model:

- create a temporary,
- then copy/move from it,
- then maybe optimize the copy away.

That mental model is no longer generally accurate.

This chapter focuses on one practical objective:

> understanding when objects are materially created, where they are created, and why copy/move constructors may never participate.

---

## 2. Scope and Non-Goals

### 2.1 Scope

We cover:

1. Copy elision before and after C++17.
2. prvalue materialization model in C++17+.
3. Guaranteed vs non-guaranteed elision contexts.
4. API design consequences.
5. Pitfalls in benchmarking and debugging assumptions.

### 2.2 Non-Goals

We do not provide an exhaustive historical survey of every wording change.
The focus is engineering-level correctness and maintainable reasoning.

---

## 3. Terminology You Must Keep Precise

### 3.1 Copy Elision

A language rule/permission where a copy or move operation is omitted by constructing directly in target storage.

### 3.2 NRVO and RVO

- **RVO** (Return Value Optimization): returning a temporary from function.
- **NRVO** (Named Return Value Optimization): returning a named local object.

Historically these were optimization opportunities; in C++17, some cases became guaranteed semantics.

### 3.3 Materialization

A prvalue may represent a pure computation result without immediate object materialization.
Materialization occurs only when object identity/storage is required.

---

## 4. Pre-C++17 vs C++17+ Mental Models

### 4.1 Pre-C++17 (Simplified)

Typical teaching narrative:

1. produce temporary object,
2. move/copy into destination,
3. compiler may optimize away.

This encouraged overemphasis on move constructor instrumentation and side effects.

### 4.2 C++17+

In key contexts, no intermediate temporary exists as a separate object.
The object is initialized directly in the final destination.

This means:

- copy/move constructor can be irrelevant,
- deleted move/copy constructors may still allow valid code,
- side-effect counting via constructors can mislead semantics discussion.

---

## 5. Guaranteed Elision: Canonical Case

```cpp
struct Widget {
    Widget() = default;
    Widget(const Widget&) = delete;
    Widget(Widget&&) = delete;
};

Widget make_widget() {
    return Widget{};
}
```

In C++17+, this is valid because no copy/move operation is required in this return form.

The destination object in caller storage is directly initialized.

---

## 6. Not Every Return Form Is Equivalent

### 6.1 Returning Prvalue Temporary

```cpp
T f() {
    return T{};
}
```

This is the classic guaranteed-elision context in C++17+.

### 6.2 Returning Named Local

```cpp
T g() {
    T obj;
    return obj; // NRVO candidate
}
```

NRVO is still commonly implemented, but reasoning should distinguish:

- what is guaranteed by language,
- what is commonly optimized by compilers,
- what your correctness depends on.

Correctness must not require NRVO success.

---

## 7. prvalue Materialization in Detail

### 7.1 prvalue Is Not Always an Object Yet

A prvalue can denote a value computation that has no immediate storage/object identity.

Materialization happens when needed, such as:

- binding to a reference requiring a temporary object,
- initializing an object with storage,
- certain member access contexts.

### 7.2 Why This Matters

This model clarifies why “counting constructor calls” is not a robust semantic explanation.
What matters is object lifetime and storage, not old temporary folklore.

---

## 8. Interactions with Deleted Move/Copy

Developers are often surprised by code that compiles without accessible copy/move operations.

### 8.1 Valid Pattern

```cpp
struct X {
    X() = default;
    X(const X&) = delete;
    X(X&&) = delete;
};

X make_x() {
    return X{}; // valid (C++17+ guaranteed direct initialization)
}
```

### 8.2 Invalid Assumption

Assuming every return path depends on move constructor availability is incorrect in modern language modes.

---

## 9. `std::move` in Return Statements

### 9.1 Common Anti-Pattern

```cpp
T f() {
    T t;
    return std::move(t); // often harms NRVO opportunities
}
```

Forcing `std::move` can change value category and interfere with copy-elision-friendly forms.

### 9.2 Guideline

Prefer plain `return t;` unless a specific reason requires explicit move semantics.

---

## 10. Constructor Side Effects Are Not Semantic Proof

Some code uses constructor counters/logging to “prove” language behavior.
This is fragile:

- optimization levels differ,
- debug and release behavior differ,
- side effects can be reordered or removed under as-if rule constraints.

Use side effects for observability, not as language-law proof.

---

## 11. API Design Consequences

### 11.1 Return by Value Is Often the Right Default

With modern elision semantics, returning by value is frequently both clear and efficient.

### 11.2 Do Not Over-Engineer for Manual Moves

Manual move plumbing in return paths can reduce readability and accidentally block optimizations.

### 11.3 Keep Correctness Independent of Optimization Luck

Even where compilers perform NRVO aggressively,
program correctness should not depend on whether NRVO triggers.

---

## 12. Exception Safety and Elision

Direct initialization in destination also affects how we reason about partially constructed objects.
Exception safety still relies on normal construction/destruction guarantees,
not on assumptions about hidden temporaries.

Design invariant:

1. constructors establish class invariants,
2. destructors release owned resources,
3. value-return APIs remain exception-safe independent of move counting.

---

## 13. Benchmarking Pitfalls

### 13.1 Debug Build Misinterpretation

Debug builds may disable or reduce optimizations,
making constructor traces look unlike production behavior.

### 13.2 Microbenchmark Trap

Benchmarking tiny artificial examples can overfit to specific compiler heuristics.
Prefer representative workloads and realistic optimization flags.

### 13.3 Tooling Suggestion

Use a combination of:

- optimized builds,
- compiler explorer style inspection,
- profiler data on real code paths.

---

## 14. Subtle Pitfalls in Teaching and Code Review

1. Saying “compiler optimized away move” where standard mandates direct construction.
2. Treating NRVO as guaranteed in all cases.
3. Introducing `std::move` in returns by habit.
4. Assuming constructor print order is portable evidence.

Correct review language should distinguish mandated semantics from optional optimization.

---

## 15. Version Awareness Checklist

Before concluding behavior, verify:

1. Is the project compiled as C++17/C++20/C++23?
2. Is the code path in guaranteed-elision context?
3. Are you discussing language semantics or implementation choices?
4. Is any conclusion based purely on debug logs?

If any answer is uncertain, restate conclusion with explicit constraints.

---

## 16. Worked Comparison

```cpp
struct T {
    T();
    T(const T&);
    T(T&&);
};

T a() {
    return T{}; // guaranteed direct construction context (C++17+)
}

T b() {
    T t;
    return t;   // NRVO candidate
}

T c() {
    T t;
    return std::move(t); // explicit xvalue path, can inhibit NRVO
}
```

Reasoning quality depends on distinguishing these three forms clearly.

---

## 17. Engineering Guidelines

1. Prefer clear value-return APIs.
2. Avoid habitual `std::move` in return statements.
3. Do not encode correctness assumptions in copy/move side effects.
4. Document standard version when teaching or reviewing semantics.
5. Treat NRVO as performance benefit, not correctness requirement.

---

## 18. Review Checklist for Production Code

Use this checklist during review:

- [ ] Return statements use semantically appropriate form.
- [ ] No unnecessary `std::move` in local return path.
- [ ] Copy/move deletion does not accidentally block required operations.
- [ ] Behavior claims are standard-version-qualified.
- [ ] Performance conclusions are based on realistic builds and data.

---

## 19. Conclusion

In modern C++, copy elision and prvalue materialization are part of the language model, not merely compiler cleverness.

The practical standard for engineering decisions is:

- reason from object lifetime and storage,
- separate guaranteed semantics from optional optimization,
- write APIs whose correctness does not depend on constructor-count folklore.

When this discipline is followed, value semantics remain both expressive and efficient.
