# RAII and Exception Safety

## 1. Introduction

Resource Acquisition Is Initialization (RAII) is one of the most fundamental design principles in C++.
It is not a naming convention and not merely an idiom to “avoid leaks.”

RAII encodes a stronger invariant:

> resource lifetime is bound to object lifetime, and object lifetime is governed by language semantics.

This matters because exception handling introduces non-linear control flow.
Without deterministic cleanup, exception paths become correctness hazards.

This chapter focuses on:

1. the language guarantees that make RAII reliable,
2. the relationship between RAII and exception safety levels,
3. practical design patterns for production C++ code.

---

## 2. Scope and Non-Goals

### 2.1 Scope

We cover:

- stack unwinding and deterministic destruction,
- RAII wrappers for common resource classes,
- basic/strong/no-throw guarantees,
- transaction-like commit patterns,
- common anti-patterns and code review checklists.

### 2.2 Non-Goals

We do not attempt to catalog every standard library utility.
The target is engineering reasoning, not API memorization.

---

## 3. Language Guarantees RAII Relies On

RAII works because C++ gives deterministic destruction rules.

### 3.1 Automatic Storage Duration Objects

Objects with automatic storage duration are destroyed when scope exits,
regardless of whether exit is normal or via exception.

### 3.2 Reverse Destruction Order

Within a scope, objects are destroyed in reverse order of construction.
This creates a natural stack discipline for nested resources.

### 3.3 Exception During Unwinding

If stack unwinding is active and another exception escapes a destructor,
the program calls `std::terminate`.

This is why destructors should be `noexcept` by design.

---

## 4. The Core RAII Shape

A robust RAII type typically has:

1. constructor that acquires resource or throws,
2. destructor that releases resource and never throws,
3. clear ownership semantics (copy/move policy explicit).

Example:

```cpp
#include <cstdio>
#include <stdexcept>

class FileHandle {
public:
    explicit FileHandle(const char* path)
        : fp_(std::fopen(path, "w")) {
        if (!fp_) {
            throw std::runtime_error("fopen failed");
        }
    }

    ~FileHandle() noexcept {
        if (fp_) {
            std::fclose(fp_);
        }
    }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept : fp_(other.fp_) {
        other.fp_ = nullptr;
    }

    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this == &other) return *this;
        if (fp_) std::fclose(fp_);
        fp_ = other.fp_;
        other.fp_ = nullptr;
        return *this;
    }

    std::FILE* get() const noexcept { return fp_; }

private:
    std::FILE* fp_{};
};
```

---

## 5. Exception Safety Levels

### 5.1 Basic Guarantee

Operation may fail, but:

1. no resource leaks,
2. object invariants remain valid.

### 5.2 Strong Guarantee

Operation is transactional:

- either succeeds completely,
- or has no observable effect.

### 5.3 No-Throw Guarantee

Operation promises not to throw.
Often required for destructors, swaps, and low-level infrastructure primitives.

RAII enables these guarantees structurally,
but does not automatically imply strong guarantee without proper algorithm design.

---

## 6. RAII as a Structural Primitive, Not a Complete Policy

RAII ensures cleanup.
It does not alone define rollback semantics for partially applied mutations.

For strong guarantee, operations often need:

1. shadow state construction,
2. commit step,
3. non-throwing swap or pointer flip.

---

## 7. Strong Guarantee via Commit Pattern

Illustrative pattern:

```cpp
#include <vector>
#include <utility>

class Buffer {
public:
    void replace_all(const std::vector<int>& src) {
        std::vector<int> tmp = src; // may throw
        data_.swap(tmp);             // no-throw for vector swap in usual alloc scenarios
    }

private:
    std::vector<int> data_;
};
```

If copy construction of `tmp` throws, `data_` remains unchanged.

---

## 8. `noexcept` and RAII Destructors

Destructors should be non-throwing in practice.
If resource release may fail,
report error through explicit API (`close()`, status object, logging),
not via throwing destructor.

Pattern:

```cpp
class Socket {
public:
    void close();          // explicit, can report errors
    ~Socket() noexcept;    // best-effort cleanup path
};
```

---

## 9. Ownership Models and RAII Types

### 9.1 Unique Ownership

Use `std::unique_ptr`, file-handle wrappers, lock guards.

### 9.2 Shared Ownership

Use `std::shared_ptr` when ownership is genuinely shared.
Avoid using shared ownership to hide unclear design.

### 9.3 Borrowing

Borrowing references/pointers should not perform cleanup.
Only owners release resources.

---

## 10. Standard RAII Wrappers You Should Prefer

1. `std::unique_ptr` for heap ownership.
2. `std::shared_ptr` for explicit shared lifetime graphs.
3. `std::lock_guard` / `std::scoped_lock` for mutex ownership.
4. `std::vector` / `std::string` for dynamic storage.
5. `std::jthread` (C++20) for thread lifecycle with cooperative stop support.

Prefer these before writing custom wrappers.

---

## 11. Lock RAII and Exception Safety

```cpp
#include <mutex>

std::mutex m;
int shared_value = 0;

void update() {
    std::lock_guard<std::mutex> g(m);
    // if exception is thrown later, lock is still released
    ++shared_value;
}
```

Without RAII lock guard, early returns and exceptions easily create deadlocks.

---

## 12. Multi-Resource Acquisition

When multiple resources are needed,
prefer constructing independent RAII objects in sequence.

If acquisition of resource N fails,
already-constructed RAII objects automatically release resources 1..N-1.

This avoids hand-written cleanup ladders.

---

## 13. Partial Construction and Invariants

If constructor throws,
fully constructed subobjects are destroyed automatically.

Design guideline:

1. establish class invariant by end of successful constructor,
2. avoid “half-valid but usable” states,
3. put ownership members in types that self-clean.

---

## 14. RAII for C APIs

Many production codebases interface with C libraries.
RAII wrappers are critical here.

Example pointer with custom deleter:

```cpp
#include <memory>

extern "C" {
    struct CHandle;
    CHandle* c_open();
    void c_close(CHandle*);
}

using CHandlePtr = std::unique_ptr<CHandle, void(*)(CHandle*)>;

CHandlePtr make_handle() {
    CHandle* raw = c_open();
    if (!raw) throw std::runtime_error("c_open failed");
    return CHandlePtr(raw, &c_close);
}
```

---

## 15. Common Anti-Patterns

1. Raw `new` in one function, `delete` in another distant function.
2. Manual lock/unlock around code that can throw.
3. Destructors that throw exceptions.
4. Hidden shared ownership causing retention cycles.
5. “Two-phase init” that requires separate `init()` after constructor.

Each anti-pattern weakens exception safety guarantees.

---

## 16. Two-Phase Initialization: Why It Is Risky

Pattern:

```cpp
class Bad {
public:
    Bad();
    void init(); // may fail
};
```

This can create objects that exist but are not usable.

Prefer:

1. constructor that establishes invariant or throws,
2. factory function returning fully formed object.

---

## 17. Factories and Error Reporting

When constructors become too complex,
use static factory functions:

```cpp
class Service {
public:
    static Service create(/* params */); // throws or returns expected-like wrapper
};
```

Factory can orchestrate multiple RAII components while preserving single-point invariant establishment.

---

## 18. RAII and Performance Myths

Myth: RAII is slower because of extra objects.

Reality:

1. wrappers are usually zero/low-overhead abstractions,
2. compiler optimizations eliminate trivial scaffolding,
3. cost of correctness bugs typically dominates micro-overhead.

Measure before assuming overhead.

---

## 19. Exception-Neutral Generic Code

Generic code should generally be exception-neutral:

- it does not swallow errors silently,
- it preserves invariants if called operations throw,
- it composes with user-provided types.

RAII is foundational to exception neutrality.

---

## 20. Moved-From States and RAII

Moved-from objects must remain valid and destructible.
RAII wrappers should define clear moved-from behavior,
typically “empty owner” state.

Ambiguous moved-from semantics often cause latent bugs.

---

## 21. Concurrency Context

RAII does not replace synchronization design,
but it guarantees release semantics even on exceptional exits.

Typical safe combination:

1. lock via RAII guard,
2. mutate state,
3. unlock automatically on all exits.

---

## 22. Testing RAII Contracts

Recommended tests:

1. force constructor failure paths,
2. inject exceptions between acquisition and normal release,
3. verify no leaks via sanitizers,
4. verify post-failure invariants remain valid.

“No crash” is insufficient proof.

---

## 23. Review Checklist

Before merging resource-owning code:

- [ ] Is ownership represented by types, not comments?
- [ ] Are destructors effectively `noexcept`?
- [ ] Are copy/move semantics explicit and correct?
- [ ] Is cleanup automatic on all control paths?
- [ ] Are strong/basic guarantees documented per operation?
- [ ] Are C resources wrapped immediately after acquisition?

If any item is unclear, redesign before optimization.

---

## 24. Practical Guidelines Summary

1. Bind every owning resource to exactly one RAII owner by default.
2. Use standard library RAII types first.
3. Keep destructors non-throwing.
4. Use commit/rollback patterns for strong guarantee.
5. Avoid two-phase initialization for required invariants.
6. Treat ownership and lifetime as first-class API contracts.

---

## 25. Conclusion

RAII transforms exception handling from ad-hoc cleanup logic into type-level correctness.

In robust C++ systems:

1. resources are owned by objects,
2. ownership transfer is explicit,
3. cleanup is deterministic,
4. exception guarantees are designed, not accidental.

When these conditions are met,
exception safety becomes a composable property of architecture,
not a fragile afterthought in control-flow branches.

## See Also

- **Related docs:** [Ownership and Lifetime](ownership-and-lifetime.md), [Undefined Behavior](undefined-behavior.md), [Noexcept and Move Operations](noexcept-and-move-operations.md)
- **Pitfalls:** [Throwing from Destructor](../pitfalls/en/throwing-from-destructor.md), [Uninitialized Members](../pitfalls/en/uninitialized-members.md)
- **Examples:** [raii-and-exception-safety.cpp](../examples/raii-and-exception-safety.cpp)
