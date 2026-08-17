# Deadlock

## 1. Introduction

A **deadlock** is a situation where two or more threads are permanently blocked,
each waiting for a resource (typically a lock) held by another thread in the
cycle. None of them can ever proceed, and the program hangs.

Deadlocks require all four of the **Coffman conditions** to hold
simultaneously:

1. **Mutual exclusion** — a resource can be held by only one thread at a time.
2. **Hold and wait** — a thread holds a resource while waiting for another.
3. **No preemption** — a held resource cannot be forcibly taken away.
4. **Circular wait** — a cycle of threads, each waiting on the next.

Breaking any one of these four conditions prevents deadlock.

## 2. The Problem

```cpp
#include <mutex>
#include <thread>

std::mutex mtx_a;
std::mutex mtx_b;

void thread_one() {
    std::lock_guard<std::mutex> la(mtx_a);
    // ... do some work while holding mtx_a ...
    std::lock_guard<std::mutex> lb(mtx_b);  // waits for mtx_b
    // use both resources
}

void thread_two() {
    std::lock_guard<std::mutex> lb(mtx_b);   // note the reversed order
    // ... do some work while holding mtx_b ...
    std::lock_guard<std::mutex> la(mtx_a);   // waits for mtx_a
    // use both resources
}
```

### Why it's broken

If `thread_one` acquires `mtx_a` and `thread_two` acquires `mtx_b`, then each
thread blocks forever waiting for the other's lock — a circular wait. This is
the classic **lock-ordering** deadlock. The bug is subtle because it happens
only with unlucky timing, not on every run.

## 3. The Fix

### 3.1 Always lock in a consistent global order

If every thread acquires `mtx_a` before `mtx_b` everywhere in the codebase, the
cycle can never form. This is simple but easy to violate as a project grows.

### 3.2 Use `std::lock` / `std::scoped_lock` (C++17) to lock atomically

```cpp
#include <mutex>

std::mutex mtx_a;
std::mutex mtx_b;

void safe_transfer() {
    // Acquires both locks deadlock-free, regardless of call order.
    std::scoped_lock lock(mtx_a, mtx_b);
    // use both resources
}
```

`std::scoped_lock` (and the lower-level `std::lock`) acquire all arguments in a
deadlock-free way using a well-known algorithm, so you no longer have to reason
about ordering by hand.

## 4. Best Practices

1. **Acquire multiple locks with `std::scoped_lock`** instead of nested
   `lock_guard`s.
2. **Keep critical sections small** to reduce the time locks are held.
3. **Avoid calling unknown code while holding a lock** — it may try to take
   another lock and create a cycle.
4. **Consider lock-free or single-owner designs** to eliminate the problem.
5. **Detect with tools** — ThreadSanitizer can report potential deadlocks.

## 5. Summary

- A deadlock needs all four Coffman conditions; break any one to prevent it.
- The most common cause is **inconsistent lock ordering**.
- `std::scoped_lock` / `std::lock` acquire multiple mutexes deadlock-free.
- Minimize held locks and avoid reentrant lock acquisition.
