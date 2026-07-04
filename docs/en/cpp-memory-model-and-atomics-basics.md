# C++ Memory Model and Atomics Basics

## 1. Introduction

Concurrent C++ correctness cannot be established by “it works on my machine.”
The C++ memory model defines when reads are allowed to observe writes,
which reorderings are legal,
and when behavior is simply undefined.

The single most important rule is:

> A data race is undefined behavior.

This chapter focuses on engineering-safe foundations:

1. how to reason with happens-before,
2. what each memory order actually promises,
3. which low-level patterns are valid and why.

---

## 2. Scope and Non-Goals

### 2.1 Scope

We cover:

- data race definition and consequences,
- atomic operations and ordering semantics,
- release/acquire publication patterns,
- read-modify-write operations,
- practical design and review checklists.

### 2.2 Non-Goals

We do not attempt to build complete lock-free data structures in this chapter.
The goal is correct foundations, not advanced algorithm catalog.

---

## 3. Core Vocabulary

### 3.1 Threads of Execution

Each thread has its own evaluation order (“sequenced-before” relation locally).
Cross-thread visibility requires explicit synchronization.

### 3.2 Data Race

A data race exists if:

1. two conflicting operations access the same memory location,
2. at least one is a write,
3. they are in different threads,
4. and they are not ordered by synchronization/happens-before.

If a data race exists, program behavior is undefined.

### 3.3 Happens-Before

`A` happens-before `B` means effects of `A` are visible to `B` under language rules.
This is the central proof tool for concurrent correctness.

### 3.4 Modification Order

For each atomic object, all writes to that object form a single total order.
This order is per-atomic-object, not global across all atomics.

---

## 4. Atomic Types: What They Guarantee

`std::atomic<T>` provides atomic operations for supported `T`.

Atomicity means operations on that object are indivisible with respect to other threads.
Atomicity alone is not enough for visibility ordering of non-atomic data.

Example:

```cpp
std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);
```

This guarantees atomic increment of `counter`.
It does not automatically order unrelated memory accesses.

---

## 5. Memory Orders at a Glance

### 5.1 `memory_order_relaxed`

- atomicity only,
- no synchronization with other operations,
- useful for statistics/counters where ordering is irrelevant.

### 5.2 `memory_order_release`

- used on stores (or RMW with release semantics),
- ensures prior writes in same thread become visible to a matching acquire observer.

### 5.3 `memory_order_acquire`

- used on loads (or RMW with acquire semantics),
- prevents subsequent reads/writes from moving before the acquire,
- synchronizes with a corresponding release on same atomic object.

### 5.4 `memory_order_acq_rel`

- used on read-modify-write operations,
- combines both acquire and release properties.

### 5.5 `memory_order_seq_cst`

- strongest commonly used order,
- establishes a single global total order for sequentially consistent atomics,
- easiest to reason about, often slower on some architectures.

---

## 6. Publication Pattern (Correct Baseline)

```cpp
#include <atomic>

std::atomic<bool> ready{false};
int payload = 0;

void producer() {
    payload = 42;
    ready.store(true, std::memory_order_release);
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)) {
    }
    // payload == 42 is now guaranteed visible
}
```

Why it works:

1. `payload = 42` is sequenced-before release-store in producer.
2. acquire-load observes the release-store value.
3. synchronization edge is established.
4. writes before release become visible after acquire.

---

## 7. Incorrect Variant: Relaxed Flag for Publication

```cpp
void producer_bad() {
    payload = 42;
    ready.store(true, std::memory_order_relaxed); // insufficient
}

void consumer_bad() {
    while (!ready.load(std::memory_order_relaxed)) {
    }
    // payload visibility is not guaranteed
}
```

Even if this appears to work on some hardware,
the language does not guarantee visibility ordering here.

---

## 8. Read-Modify-Write Operations

RMW operations include:

- `fetch_add`, `fetch_sub`,
- `exchange`,
- `compare_exchange_weak/strong`.

These operations are atomic with respect to competing operations on same atomic object.

Example:

```cpp
std::atomic<int> x{0};
x.fetch_add(1, std::memory_order_acq_rel);
```

Use `acq_rel` when operation both consumes prior synchronization and publishes subsequent state.

---

## 9. CAS Loop Pattern

```cpp
std::atomic<int> value{0};

void increment_if_even() {
    int expected = value.load(std::memory_order_relaxed);
    while ((expected % 2) == 0) {
        if (value.compare_exchange_weak(
                expected,
                expected + 1,
                std::memory_order_acq_rel,
                std::memory_order_relaxed)) {
            break;
        }
        // expected updated on failure
    }
}
```

Key points:

1. `compare_exchange_weak` may fail spuriously.
2. failure memory order must be no stronger than success order.
3. loop structure must tolerate retries.

---

## 10. Fences: Powerful but Easy to Misuse

`atomic_thread_fence` can impose ordering without directly touching a specific atomic object.
However, fence-based designs are harder to prove and review.

Engineering recommendation:

1. prefer operation-associated orders (`store(..., release)`, `load(..., acquire)`),
2. use explicit fences only with strong justification and documentation.

---

## 11. `volatile` Is Not Synchronization

In C++, `volatile` is primarily for special memory access semantics (e.g., MMIO contexts).
It does **not** provide inter-thread synchronization guarantees.

Do not replace atomics/mutexes with `volatile` for concurrency correctness.

---

## 12. Atomicity vs Composite Invariants

Atomic operations protect individual memory locations.
They do not automatically protect multi-variable invariants.

If consistency requires multiple fields to move together,
use:

1. a lock,
2. or a formally verified lock-free protocol with explicit ordering proof.

---

## 13. Architecture Myths

“x86 is strong, so this is safe” is not a language proof.

Correctness must be proven in C++ memory-model terms,
not by relying on one CPU family’s observed behavior.

Portability failures often appear only after:

- architecture change,
- compiler upgrade,
- optimization level change.

---

## 14. Choosing an Initial Memory Order Strategy

Practical strategy for teams:

1. start with `seq_cst` for correctness clarity,
2. write tests and invariants,
3. relax to acquire/release or relaxed only with proof,
4. benchmark after correctness is established.

Premature weakening of memory order is a common source of latent bugs.

---

## 15. Testing and Verification Reality

Concurrent bugs are schedule-dependent and non-deterministic.
Absence of failure is not proof of correctness.

Use layered validation:

1. stress tests with randomized scheduling pressure,
2. ThreadSanitizer for race detection,
3. model-level reasoning in code review,
4. assertions for protocol state transitions.

---

## 16. Typical Safe Use Cases for Relaxed Order

`memory_order_relaxed` is acceptable when:

1. only atomicity matters,
2. no thread uses that atomic as publication/synchronization signal,
3. stale value observations are acceptable by design.

Common example: telemetry counters.

```cpp
std::atomic<unsigned long long> events{0};

void record() {
    events.fetch_add(1, std::memory_order_relaxed);
}
```

---

## 17. Typical Publication Pattern with Pointer

```cpp
struct Config {
    int threshold;
};

std::atomic<Config*> g_cfg{nullptr};

void publish(Config* p) {
    g_cfg.store(p, std::memory_order_release);
}

Config* acquire_cfg() {
    return g_cfg.load(std::memory_order_acquire);
}
```

Consumer observing non-null pointer via acquire load
also observes prior initialization performed before release store.

---

## 18. When to Prefer Mutex Over Atomics

Use mutex if:

1. multiple fields must remain consistent together,
2. protocol complexity exceeds easy local proof,
3. maintenance/readability is a priority,
4. contention profile does not justify lock-free complexity.

Lock-free is not automatically faster in end-to-end systems.

---

## 19. Review Checklist

Before merging atomic code, verify:

1. Is there a clear happens-before proof?
2. Are data-race-free guarantees explicit?
3. Are memory orders minimal but sufficient?
4. Is `relaxed` used only where ordering is irrelevant?
5. Are architecture assumptions avoided?
6. Is fallback to mutex considered and documented?

If proof is unclear, simplify design.

---

## 20. Terminology Precision in Documentation

Use terms precisely:

- “undefined behavior” for data races,
- “synchronizes-with” for release/acquire edges,
- “happens-before” for visibility guarantee.

Avoid vague wording like “likely visible” in technical docs.

---

## 21. Version Notes

- C++11 introduced formal memory model and atomics.
- Later standards expanded atomic support and utility APIs.
- Core correctness principles in this chapter remain stable across modern standards.

Always state language mode in reproducible examples.

---

## 22. Conclusion

Correct concurrent C++ begins with language-level reasoning, not processor folklore.

Engineering sequence should be:

1. define invariants,
2. establish happens-before relations,
3. choose memory orders that satisfy proof,
4. validate with tooling and stress tests,
5. optimize only after correctness is locked.

When this discipline is followed,
atomics become a precise tool rather than a source of nondeterministic defects.
