# Coroutines: Lifetime and Suspension Points

## 1. Introduction

C++20 coroutines are stackless state machines with explicit suspension points.
Their complexity is not primarily about syntax (`co_await`, `co_return`, `co_yield`),
but about lifetime boundaries that no longer align with ordinary lexical scope intuition.

In synchronous functions, most reasoning can be done by reading scope structure.
In coroutines, execution can pause, escape the current call stack, and resume later.
Therefore:

> coroutine correctness is fundamentally lifetime correctness across temporal gaps.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This chapter focuses on:

1. what lives in coroutine frame,
2. what can dangle across suspension,
3. how cancellation/error channels affect lifetime,
4. engineering patterns for safe coroutine APIs.

### 2.2 Non-Goals

This is not a full primer on implementing custom awaiters and full scheduler runtimes.
We only include details that materially change correctness decisions.

---

## 3. Minimal Semantic Model

### 3.1 Coroutine as State Machine

A coroutine function is transformed into:

- a coroutine frame (heap or custom-allocated storage),
- a `promise_type` object,
- resume/suspend points and state transitions.

### 3.2 Frame Contents (Conceptual)

The frame typically contains:

1. `promise_type` state,
2. function parameters,
3. local variables that must survive suspension,
4. bookkeeping required by compiler/runtime.

Locals that provably do not cross suspension may remain transient.

### 3.3 Destroying the Frame

When coroutine completes or is explicitly destroyed,
the frame is destroyed and all frame-resident objects are destroyed.

Any outstanding handles/references into that frame become invalid.

---

## 4. Suspension Points Change Lifetime Assumptions

`co_await expr` may do one of two things:

1. continue synchronously (no suspension),
2. suspend and return control to caller/scheduler.

Correct code must be safe in both paths.

A frequent mistake is assuming “this await usually resumes immediately,”
then writing logic that only works in the non-suspending path.

---

## 5. Parameters, References, and Borrowed Views

### 5.1 Value Parameters

Passing by value into coroutine usually gives robust ownership semantics.
If value must survive suspension, it is available in frame.

### 5.2 Reference Parameters

Reference parameters are non-owning aliases.
They are safe only if referenced objects outlive all coroutine resumptions that access them.

### 5.3 View Types (`std::string_view`, spans, iterators)

View types are borrowing abstractions.
Cross-suspension use is safe only when the underlying storage lifetime is guaranteed.

---

## 6. Canonical Dangling Example

```cpp
// conceptual pseudo-task type
task<void> bad(std::string_view sv) {
    co_await some_async_op();
    use(sv); // UB if sv points to destroyed storage
}
```

If caller passes a temporary `std::string`, `sv` can dangle after suspension.
This bug may appear nondeterministically under load or on different schedulers.

---

## 7. Safer Alternative: Own Data Across Suspension

```cpp
task<void> good(std::string s) {
    co_await some_async_op();
    use(s); // owns storage, safe across suspension
}
```

By-value parameter trades some copying/moving for explicit lifetime control.
In production systems, correctness dominates micro-optimization assumptions.

---

## 8. Captures and Coroutine Lambdas

A lambda that becomes a coroutine has capture lifetime pitfalls.

```cpp
auto make_task() {
    int x = 42;
    return [&, x_copy = x]() -> task<void> {
        co_await some_async_op();
        use(x_copy); // safe
        // using by-reference capture of local from outer scope can dangle
    }();
}
```

Rule of thumb:

- if callable may outlive current scope, avoid reference capture unless lifetime is externally guaranteed.

---

## 9. Awaiter Lifetime Hazards

`co_await expr` conceptually uses an awaiter object.
If awaiter stores references to ephemeral objects, resumption can access invalid data.

Design requirements for awaiters:

1. references must target stable storage,
2. resumption callbacks must not outlive captured resources,
3. destruction path must be idempotent and exception-safe.

---

## 10. Thread/Executor Affinity Is a Contract, Not a Guess

Coroutines do not inherently guarantee same-thread resumption.
Resumption policy depends on awaiter/executor design.

Therefore APIs should document:

1. where continuation runs,
2. whether context hop can happen,
3. synchronization obligations for shared state.

Undocumented affinity assumptions are a common source of races.

---

## 11. Cancellation and Lifetime Coupling

Cancellation is not only control-flow concern; it is a lifetime event.

When cancellation occurs, ensure:

1. in-flight operations release resources exactly once,
2. continuation does not observe partially destroyed state,
3. stop-token registrations are unregistered safely.

Cancellation-aware code should be tested with forced cancellation at every await point.

---

## 12. Error Propagation and Resource Safety

Exception paths in coroutines cross suspension boundaries.
Design should ensure:

1. exceptions are transported via task result channel or rethrow strategy,
2. frame destruction still releases resources,
3. no hidden dependency on “happy path resume order.”

`final_suspend` behavior must be understood by API consumers and implementers.

---

## 13. Handle Ownership and Destruction Discipline

Coroutine handles are powerful and dangerous.

If multiple components believe they own destruction rights,
double-destroy and use-after-destroy defects become likely.

Use explicit ownership model:

- unique owner destroys handle,
- observers only inspect state,
- transfer semantics documented and tested.

---

## 14. `initial_suspend` and `final_suspend` in API Semantics

These two points affect external behavior substantially:

1. eager vs lazy start policy,
2. when completion becomes observable,
3. who is responsible for final cleanup handoff.

Poorly documented suspend policy causes integration bugs in higher layers.

---

## 15. Reentrancy and Nested Resume Risks

Some awaiters may resume continuation inline.
This can create reentrancy scenarios where code assumes non-reentrant progression.

Engineering rule:

1. never assume resume is always deferred,
2. guard mutable state transitions,
3. avoid hidden recursion-like control flow surprises.

---

## 16. Resource Types Commonly Affected

High-risk resources across suspension:

1. lock guards (`std::lock_guard`) held across await (usually design smell),
2. raw pointers into container elements that may be invalidated,
3. references to stack variables from caller scope,
4. borrowed views over temporary storage.

As a default, avoid holding locks across `co_await` unless design explicitly proves safety.

---

## 17. Debugging Reality: Heisenbugs Are Expected

Coroutine lifetime bugs are often schedule-sensitive.
Symptoms can vanish under debugger timing changes.

Use strategies:

1. deterministic test scheduler where possible,
2. stress tests with randomized delays,
3. sanitizers and race detectors,
4. explicit lifetime assertions in debug builds.

---

## 18. Documentation Template for Coroutine APIs

For each public coroutine API, document:

1. ownership of parameters and returned task,
2. whether references/views may be retained after call returns,
3. cancellation semantics,
4. execution context and thread-affinity expectations,
5. completion and destruction responsibilities.

If these are missing, users will assume incorrect defaults.

---

## 19. Practical Patterns

### 19.1 Prefer Value for Cross-Suspension Data

If data is needed after any await, value ownership is the default-safe design.

### 19.2 Split Borrowing and Owning APIs

Provide explicit overloads/contracts:

- borrowing fast path with strict lifetime precondition,
- owning safe path for general use.

### 19.3 Keep Await Boundaries Visible

Large coroutine bodies hide suspension points.
Extract operations into small functions to improve reviewability.

---

## 20. Example: Borrowing vs Owning API Pair

```cpp
// Borrowing API: caller must guarantee lifetime.
task<void> send_view(std::string_view payload);

// Owning API: safe by default.
task<void> send_owned(std::string payload);
```

This split makes cost/safety trade-off explicit and reviewable.

---

## 21. Common Anti-Patterns

1. Passing temporary-backed views into long-lived coroutine tasks.
2. Capturing references in coroutine lambdas that escape current scope.
3. Holding locks across suspension points.
4. Relying on undocumented same-thread resume behavior.
5. Treating cancellation as optional afterthought.

Each anti-pattern is fundamentally a lifetime contract violation.

---

## 22. Code Review Checklist

- [ ] Every `co_await` boundary reviewed for lifetime impact.
- [ ] No borrowed data crosses suspension without documented guarantee.
- [ ] Handle ownership and destruction path are unique and explicit.
- [ ] Cancellation path releases resources exactly once.
- [ ] Thread/executor assumptions are documented.
- [ ] Locking strategy is safe across awaits.

---

## 23. Version and Portability Notes

- Coroutines are standardized in C++20.
- Library ecosystem (`task` types, executors, cancellation models) varies.
- Do not confuse one framework’s behavior with language-wide guarantee.

Portability requires separating core language semantics from runtime framework policy.

---

## 24. Conclusion

Coroutines extend function lifetime across time, scheduler boundaries, and ownership domains.
As a result, traditional “scope-only” reasoning is insufficient.

Robust coroutine engineering depends on explicit contracts for:

1. who owns data,
2. who owns continuation handle,
3. where continuation executes,
4. how cancellation and errors terminate lifetimes.

When these contracts are explicit, coroutines become maintainable.
When they are implicit, defects are inevitable and difficult to reproduce.
