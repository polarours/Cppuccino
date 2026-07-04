# Dangling Lambda Captures

## 1. Problem Statement

Lambdas can capture by reference, by value, or by mixed modes.
When a lambda outlives any object captured by reference,
its closure holds a dangling reference.

This is undefined behavior and can manifest as:

1. sporadic crashes,
2. stale or corrupted values,
3. nondeterministic behavior under optimization.

The bug is subtle because code often appears to work in local tests.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This article covers:

1. why dangling captures happen,
2. common escape scenarios,
3. async/threading amplifiers,
4. safe capture and ownership patterns,
5. practical code review checks.

### 2.2 Non-Goals

We do not discourage lambdas in general.
The goal is precise lifetime reasoning, not stylistic restrictions.

---

## 3. Minimal Failing Example

```cpp
#include <functional>

std::function<int()> make_bad() {
    int x = 7;
    return [&]() { return x; }; // captures x by reference
}
```

When `make_bad` returns, `x` is destroyed.
Calling returned function reads a dangling reference -> undefined behavior.

---

## 4. Why This Happens

A lambda closure object stores capture state.

- by-value capture stores a copy/member,
- by-reference capture stores a reference-like binding to external object.

If captured object lifetime ends before lambda invocation,
closure’s reference is invalid.

No runtime guard exists by default.

---

## 5. “Looks Fine” Is Not Proof

Developers often see stable output in debug mode and assume safety.
That is misleading because UB may remain latent until:

1. optimization changes layout/register reuse,
2. stack reuse pattern changes,
3. call timing shifts under load,
4. compiler version changes.

Correctness must be proven by lifetime rules, not observations.

---

## 6. Escape Scenarios That Commonly Trigger Dangling

### 6.1 Returning Lambda

Returning a lambda that captured local references.

### 6.2 Storing in Longer-Lived Object

Assigning lambda to class member, global callback table, event bus, or deferred work queue.

### 6.3 Async Execution

Passing lambda to thread pool, timer, coroutine continuation, or executor where execution happens later.

### 6.4 Detached Threads

Capturing local references in detached worker lambdas is especially dangerous.

---

## 7. Safe Baseline: Capture by Value for Escaping Lambdas

```cpp
#include <functional>

std::function<int()> make_good() {
    int x = 7;
    return [x]() { return x; }; // copy into closure
}
```

The closure owns its copy of `x`,
so invocation after function return remains valid.

---

## 8. Capture Defaults and Their Risks

### 8.1 `[&]`

Captures all odr-used outer variables by reference.
High risk if lambda escapes scope.

### 8.2 `[=]`

Captures by value (except historical `this` behavior nuances).
Safer for escaping callbacks, but may copy heavy objects unexpectedly.

### 8.3 Recommendation

Avoid broad capture defaults in nontrivial code.
Prefer explicit capture lists that encode ownership intent.

---

## 9. `this` Capture Pitfall

Capturing `this` does not copy object state;
it copies pointer value.

If object is destroyed before lambda runs,
`this` becomes dangling.

Risk pattern:

```cpp
class Worker {
public:
    void schedule(); // stores callback for later
private:
    int value_ = 0;
};

void Worker::schedule() {
    submit([this] { use(value_); }); // unsafe if *this may die first
}
```

---

## 10. Safer `this` Strategies

### 10.1 Capture Snapshot by Value

```cpp
submit([v = value_] { use(v); });
```

Works when only immutable snapshot is needed.

### 10.2 Shared Ownership Lifetime Extension

For asynchronously invoked member logic:

```cpp
class Worker : public std::enable_shared_from_this<Worker> {
public:
    void schedule() {
        auto self = shared_from_this();
        submit([self] { self->do_work(); });
    }
    void do_work();
};
```

Use carefully; this changes ownership/lifetime graph.

---

## 11. `std::reference_wrapper` Is Not a Fix

`std::ref(x)` stores reference semantics explicitly,
but does not extend lifetime.

If `x` dies, wrapper still dangles.

Use reference wrappers only when lifetime dominance is guaranteed externally.

---

## 12. Moving into Capture

Modern C++ supports init-capture with move:

```cpp
auto fn = [p = std::move(ptr)]() {
    if (p) use(*p);
};
```

This is a robust ownership-transfer tool for escaping lambdas.

---

## 13. Async Amplification

With async frameworks,
execution may occur much later and on different threads.

That widens timing windows for dangling capture bugs.

Design assumptions like “callback runs immediately” are unsafe unless API guarantees it.

---

## 14. Event Systems and Deferred Queues

In event-loop architectures,
callbacks are routinely stored and replayed later.

Capture rules for such systems should default to:

1. value capture for data,
2. explicit shared/weak ownership for objects,
3. no hidden reference capture defaults.

---

## 15. Weak Ownership Pattern for Callbacks

To avoid extending lifetime indefinitely:

```cpp
auto weak = weak_from_this();
submit([weak] {
    if (auto self = weak.lock()) {
        self->do_work();
    }
});
```

This avoids dangling and avoids forcing object to live forever.

---

## 16. Coroutines and Lambda Captures

Coroutine continuations often store callable state across suspension points.
Reference captures in this context are especially fragile.

If a continuation may run after caller scope exits,
capture by value or explicit ownership is required.

---

## 17. Performance Considerations (Without Sacrificing Safety)

Yes, value capture may copy data.
But reference-capture bugs usually cost more than small-copy overhead.

Optimization sequence:

1. make lifetime safe,
2. profile,
3. optimize heavy captures with move/smart pointers/immutable shared state.

---

## 18. Diagnostic Strategies

Helpful tools:

1. AddressSanitizer (detects many UAF cases),
2. UBSan for broader UB signals,
3. ThreadSanitizer for async/concurrency contexts,
4. static analysis for escaping lambda references.

No single tool catches all lifetime bugs, use layered checks.

---

## 19. Code Smells

1. Escaping lambda uses `[&]`.
2. Callback captures local references and is stored beyond scope.
3. Member callback captures raw `this` in async path.
4. Large API uses default captures with unclear ownership semantics.
5. Lifetime assumptions exist only in comments, not type design.

---

## 20. Safer Design Rules

1. If lambda may escape, default to value capture.
2. For object methods, choose `shared_ptr` or `weak_ptr` strategy explicitly.
3. Keep capture lists explicit and minimal.
4. Prefer immutable snapshots over shared mutable references.
5. Document callback lifetime contracts in API.

---

## 21. API Contract Example

Instead of vague callback docs, specify:

1. whether callback can run asynchronously,
2. whether callback may outlive caller scope,
3. required lifetime of referenced arguments,
4. thread/executor context.

Clear contracts prevent accidental dangling by API users.

---

## 22. Review Checklist

- [ ] Does the lambda escape current scope?
- [ ] Are any references captured in escaping lambda?
- [ ] Is `this` captured in deferred/async path?
- [ ] Is ownership model (`value` / `shared` / `weak`) explicit?
- [ ] Are capture defaults avoided for nontrivial callbacks?
- [ ] Are lifetime assumptions documented in API contract?

If any answer is unclear, redesign capture strategy.

---

## 23. Migration Guidance

For existing codebases:

1. search for `[&]` in callback-heavy modules,
2. classify escaping vs non-escaping lambdas,
3. convert escaping captures to value/owned forms,
4. introduce shared/weak ownership patterns where needed,
5. add sanitizer-backed async tests.

This process removes a large class of nondeterministic production bugs.

---

## 24. Summary

Dangling lambda captures are lifetime-contract failures.
The pattern is simple, but consequences are severe and nondeterministic.

Practical baseline:

1. escaping lambda -> no implicit reference capture,
2. async callback -> explicit ownership strategy,
3. `this` capture -> treat as pointer lifetime problem,
4. verify with tooling and review checklists.

Lambdas are safe and expressive when lifetime is explicit.
They become dangerous when capture semantics are implicit and assumptions are undocumented.
