# Broken Double-Checked Locking

## 1. Problem Statement

Double-Checked Locking (DCLP) is a lazy-initialization pattern intended to avoid taking a lock after initialization.
Its classic form is widely known and widely misimplemented.

The common broken version appears “reasonable” and may pass tests,
yet still has undefined behavior or visibility bugs under concurrency.

Core risk:

> one thread can observe a non-null pointer before the pointed object is fully initialized and safely published.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This article covers:

1. why classic DCLP fails,
2. memory-order requirements for correctness,
3. modern C++ alternatives that are safer and simpler,
4. practical review checklists.

### 2.2 Non-Goals

We do not advocate hand-written lock-free initialization for normal application code.
The preferred outcome is to avoid DCLP unless there is a hard, measured need.

---

## 3. Classic Broken Form

Typical (broken) sketch:

```cpp
MyType* g_ptr = nullptr;
std::mutex g_m;

MyType* instance_bad() {
    if (g_ptr == nullptr) {
        std::lock_guard<std::mutex> lk(g_m);
        if (g_ptr == nullptr) {
            g_ptr = new MyType();
        }
    }
    return g_ptr;
}
```

At first glance:

1. first check avoids lock in fast path,
2. second check prevents duplicate construction.

However, this code has data-race and publication-order problems.

---

## 4. Why It Fails: Memory Model View

There are two separate correctness requirements:

1. **uniqueness** (construct once),
2. **safe publication** (all threads see fully initialized object state).

Classic DCLP focuses on uniqueness and often misses safe publication.

Without proper atomic synchronization:

- load/store of pointer can race,
- object writes in constructor may not be visible when another thread sees non-null pointer.

Either case breaks correctness.

---

## 5. Reordering and Visibility Intuition

Even if source code says:

1. construct object,
2. assign pointer,

other threads may observe effects in a different order unless synchronization establishes happens-before.

So a reader thread might see:

- `g_ptr != nullptr`,
- but stale/default data inside `*g_ptr`.

This is exactly the “partially constructed observation” failure mode.

---

## 6. Data Race Is Already UB

If one thread writes `g_ptr` while another reads `g_ptr` unsafely,
that alone is a data race and therefore undefined behavior in C++.

This means the argument “works on x86” is irrelevant.
Language-level UB invalidates portability and reasoning.

---

## 7. Why Old Explanations Are Misleading

Historically, many discussions used CPU-specific models or folklore.
Modern C++ requires memory-model proof in terms of:

1. atomic operations,
2. synchronization edges,
3. happens-before relationships.

Anything less is not a correctness proof.

---

## 8. Correctness Conditions If You Still Implement DCLP

If DCLP must be implemented manually,
pointer publication and observation must use atomics with release/acquire semantics,
and construction path must be protected from duplicates.

Conceptual requirements:

1. publishing store is `release`,
2. consuming load is `acquire`,
3. duplicate initialization prevented under lock or CAS discipline,
4. object lifetime and destruction policy are explicit.

---

## 9. Safer Modern Replacement #1: Local Static

Preferred in most cases:

```cpp
MyType& instance() {
    static MyType obj;
    return obj;
}
```

From C++11 onward, initialization of function-local statics is thread-safe by language guarantee.

Benefits:

1. simple,
2. readable,
3. no manual memory-order logic,
4. no dynamic allocation required.

---

## 10. Safer Modern Replacement #2: `std::call_once`

Useful when initialization logic must be separated:

```cpp
#include <mutex>
#include <memory>

std::once_flag g_once;
std::unique_ptr<MyType> g_obj;

MyType& instance2() {
    std::call_once(g_once, [] {
        g_obj = std::make_unique<MyType>();
    });
    return *g_obj;
}
```

This provides one-time initialization with standard synchronization semantics.

---

## 11. Why `std::call_once` Is Often Better Than Hand-Rolled DCLP

`std::call_once` centralizes difficult concurrency details in tested library code.

Compared to manual DCLP:

1. less room for memory-order mistakes,
2. clearer intent,
3. easier code review,
4. cleaner exception handling behavior.

---

## 12. Exception Behavior in One-Time Init

If initialization callable passed to `std::call_once` throws,
the flag is not permanently set,
and future calls retry initialization.

This behavior is often desirable but should be documented.

For local statics, failed initialization similarly permits later retries.

---

## 13. Destruction and Lifetime Policy

Initialization is only half the problem.
You must also define destruction expectations:

1. process-lifetime singleton,
2. controlled shutdown singleton,
3. intentional leak-on-exit for teardown-order safety.

Many systems fail not at init, but at shutdown due to deinitialization order.

---

## 14. Static Deinitialization Hazards

Even with safe initialization,
global/static destruction order across translation units can be problematic.

If singleton A uses singleton B during teardown,
but B is already destroyed,
behavior is undefined.

Mitigation options:

1. avoid cross-static dependencies,
2. explicit shutdown ordering,
3. process-lifetime objects with no destruction dependency.

---

## 15. Performance Myth: “DCLP Is Always Faster”

In many workloads,
local static or `call_once` overhead after initialization is negligible.

Hand-written DCLP complexity often exceeds any measurable benefit.

Optimization rule:

1. first prove correctness,
2. then profile real workload,
3. only then consider lower-level alternatives.

---

## 16. If You Need Lock-Free Publication Patterns

For advanced systems,
you may require explicit atomics and lock-free publication.

Then documentation should include:

1. exact memory orders,
2. happens-before proof,
3. object lifetime ownership protocol,
4. ABA/reclamation considerations if applicable.

Without written proof, code is not reviewable at required safety level.

---

## 17. Typical Anti-Patterns

1. Non-atomic pointer load outside lock + non-atomic store inside lock.
2. Using `memory_order_relaxed` for publication.
3. Relying on “x86 is strong memory model.”
4. Mixing multiple init paths without a single ownership authority.
5. Ignoring teardown ordering.

---

## 18. Minimal Correctness Smell Tests

When reviewing lazy singleton code, ask:

1. Is there any non-atomic shared read/write race?
2. Is safe publication proven via acquire-release or stronger sync?
3. Is duplicate construction impossible?
4. Is destruction policy explicit?
5. Could simpler standard mechanisms replace this?

If answers are unclear, reject the implementation.

---

## 19. Code Review Checklist

- [ ] Prefer function-local static where feasible.
- [ ] Otherwise prefer `std::call_once`.
- [ ] Avoid custom DCLP unless hard requirement is documented.
- [ ] For custom atomic path, memory orders are justified with happens-before proof.
- [ ] Initialization failure behavior is specified.
- [ ] Shutdown/destruction ordering is specified.

---

## 20. Migration Guidance for Legacy DCLP

For legacy code:

1. identify manual DCLP sites,
2. replace with local static when possible,
3. otherwise replace with `std::call_once`,
4. add stress/concurrency tests,
5. document lifetime and teardown semantics.

This usually reduces both bug risk and code complexity.

---

## 21. Summary

Classic double-checked locking is a concurrency pitfall because it appears correct while violating memory-model requirements.

Practical rule set:

1. default to function-local static initialization (C++11+),
2. use `std::call_once` when structure requires separation,
3. avoid manual DCLP unless you can provide formal synchronization proof.

Correct concurrent C++ is built on explicit happens-before reasoning,
not on historical folklore or architecture-specific luck.
