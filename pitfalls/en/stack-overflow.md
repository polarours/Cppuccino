# Stack Overflow

## Introduction

The call stack is a finite region of memory. Every function call pushes a *stack frame* (return address, saved registers, local variables). A *stack overflow* happens when the total size of live frames exceeds the stack's limit — most often from unbounded or excessively deep recursion, but also from allocating huge objects by value on the stack.

Unlike a memory leak, a stack overflow is usually loud: the program is terminated on the spot (SIGSEGV on Linux). But it is still a real defect that must be designed away.

## The Problem

```cpp
// No base case: this will recurse until the stack is exhausted.
void countdown() {
    countdown();   // each call adds a frame; the stack eventually runs out
}
```

A subtler, very common case is *deep but legitimate-looking* recursion on large inputs:

```cpp
// Computes the sum of a[i..n) recursively.
long long sum(const std::vector<long long>& a, size_t i) {
    if (i == a.size()) return 0;
    return a[i] + sum(a, i + 1);   // depth == a.size(); fails for big vectors
}
```

**Why it's broken:**
- `countdown()` has no terminating condition, so it pushes frames forever until the OS kills the thread — a guaranteed crash.
- `sum()` works for small vectors but overflows the stack once `a.size()` exceeds a few tens of thousands of frames (typical stack is 8 MiB). The bug only appears with "large" real-world input, so it slips through small unit tests.
- Allocating a large object by value, e.g. `char buffer[1 << 20];` (1 MiB) inside a function, can also blow the stack in one frame.

## The Fix

Prefer an **iterative** formulation, moving the state from the stack into a loop or a heap-allocated structure:

```cpp
#include <vector>

long long sum(const std::vector<long long>& a) {
    long long total = 0;
    for (size_t i = 0; i < a.size(); ++i) {   // state lives in a loop, not the stack
        total += a[i];
    }
    return total;
}
```

When recursion genuinely models the problem (tree traversal, divide-and-conquer), you can:
- **Switch to an explicit stack** (a `std::vector` or `std::deque` on the heap) that you manage with a loop, so depth is limited only by available heap, or
- **Keep tail recursion** and rely on the compiler tail-call-optimizing it to a loop (verify with `-O2` and a tool/inspection — it is not guaranteed in C++), or
- **Increase the stack size** only as a last resort for a truly deep but bounded recursion (e.g. `ulimit -s` / `pthread_attr_setstacksize`).

Also avoid giant by-value locals; use `std::vector<char>` (heap-backed) instead of a huge array.

## Best Practices

- **Recursion needs a base case *and* a provably bounded depth.*** "It terminates" is not enough; the depth must fit the stack for your largest expected input.
- **Replace deep linear recursion with loops.** Summing, copying, searching a flat range — these are almost always clearer and safer iteratively.
- **For tree/graph recursion, consider an explicit (heap) work stack** when depth can be large (e.g. a degenerate linked list posing as a tree).
- **Beware large stack allocations.** A multi-megabyte local array or `std::array` is a single-frame overflow waiting to happen; put big buffers on the heap.
- **Let the compiler warn.** Build with `-Wstack-usage=<bytes>` to get a diagnostic whenever a function's stack use may exceed your budget.

## Summary

Stack overflow is the stack telling you it has run out of room for call frames. The fix is rarely "make the stack bigger"; it is to stop pushing unbounded frames: convert deep or open-ended recursion into iteration or an explicit heap-backed stack, and keep large objects off the stack. Design for the *largest* input, not just the one in your test suite.
