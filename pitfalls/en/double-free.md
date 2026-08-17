# Double Free

## Introduction

A *double free* occurs when `delete` is called on the same pointer (or a copy of it) more than once. The result is **undefined behavior**: the memory allocator's internal bookkeeping gets corrupted, which can crash immediately, corrupt unrelated data, or open a security hole that an attacker can exploit to take over the process.

Double frees are especially sneaky because the program often *appears* to work for a while — the corruption may only bite much later, far from the buggy line.

## The Problem

```cpp
void cleanup() {
    int* p = new int(42);
    int* q = p;          // (1) q aliases p — but does NOT own a separate object

    delete p;            // (2) the int is freed
    delete q;            // (3) DOUBLE FREE: q points to the same already-freed int -> UB
}
```

A more realistic variant passes a raw pointer into a function that "helpfully" deletes it, and the caller also deletes:

```cpp
void consume(int* data) {
    // ... use data ...
    delete data;         // (a) consumes ownership, caller must NOT delete again
}

void caller() {
    int* x = new int(7);
    consume(x);
    delete x;            // (b) DOUBLE FREE: x was already freed inside consume()
}
```

**Why it's broken:**
- `q` in the first example is just another name for the same allocation. `delete q` frees memory that is already back in the allocator's free list.
- The allocator may now hand that same block to a later `new`, so the second "free" and a later "alloc" corrupt each other's metadata. There is no language-level safety net — the compiler will not warn, and the program may pass your tests before failing in production.

## The Fix

The only robust fix is to make ownership explicit so a pointer is freed **exactly once, automatically**:

```cpp
#include <memory>

void consume(std::unique_ptr<int> data) {
    *data += 1;   // unique_ptr owns the int; we can read/modify it freely
    // ... and it is freed automatically when the function returns ...
}   // data is destroyed here; freed exactly once, no matter what

void caller() {
    auto x = std::make_unique<int>(7);
    consume(std::move(x));   // ownership transferred; x is now empty
    // deleting x again is impossible — it owns nothing
}
```

If you cannot move to smart pointers yet, follow the strict discipline: **a raw pointer is either borrowed (never deleted by the borrower) or owned by exactly one site (deleted exactly once).** Document which, and never let two sites believe they own the same raw pointer.

## Best Practices

- **Own with `std::unique_ptr`; pass with `std::unique_ptr` or a raw observer.** When a function takes ownership, take it *by value* as `std::unique_ptr` — the type itself prevents a double free.
- **Never `delete` a raw pointer you did not allocate, and never `delete` the same raw pointer twice.** This sounds obvious, but it is exactly what aliasing and "helpful" deleters break.
- **`std::shared_ptr` is not a license to double-free.** Two `shared_ptr`s must share *one* control block (created together via `make_shared` or one copy-constructed from the other). Reconstructing a `shared_ptr` from the same raw pointer yields two independent owners → double free. Always copy the `shared_ptr`, never re-wrap the raw `get()`.
- **Use `= delete` on dangerous overloads.** If a class manages a resource, delete its copy operations or implement them properly; a silently-copied raw pointer is a double-free waiting to happen.
- **Catch it with sanitizers.** AddressSanitizer (`-fsanitize=address`) detects double frees at runtime and points straight at both `delete` sites.

## Summary

A double free is what happens when two pieces of code each believe they own the same memory. Solve it by making ownership *unambiguous and unique*: one `std::unique_ptr` per resource, moved (not copied, not aliased) across boundaries. When ownership is encoded in the type system, freeing twice becomes impossible instead of merely "something to remember."
