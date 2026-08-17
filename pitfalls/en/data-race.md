# Data Race

## 1. Introduction

A **data race** occurs when two or more threads access the same memory location
concurrently, at least one of the accesses is a write, and there is no
synchronization operation ordering those accesses. In C++, a data race is
**undefined behavior** — the program may produce any result, crash, or appear to
work correctly most of the time and fail only occasionally.

This is one of the most common and most dangerous concurrency bugs, because it
is often invisible in ordinary testing and only manifests under specific
scheduling, on specific hardware, or after an innocent-looking compiler
optimization.

## 2. The Problem

```cpp
#include <thread>
#include <vector>

int counter = 0;

void worker() {
    for (int i = 0; i < 100'000; ++i) {
        ++counter;  // read-modify-write on a shared variable, no synchronization
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int t = 0; t < 10; ++t)
        threads.emplace_back(worker);
    for (auto& th : threads)
        th.join();

    // Expected: counter == 1'000'000
    // Reality:  often a smaller number, and formally undefined behavior
}
```

### Why it's broken

`++counter` is not atomic. It expands into three operations: load `counter`
from memory, add 1, store it back. When two threads execute it concurrently:

- Thread A reads `counter == 5`
- Thread B reads `counter == 5`
- Thread A writes `6`
- Thread B writes `6`

The increment from one thread is lost. Worse, because a data race is undefined
behavior, the compiler is free to optimize the loop assuming no other thread
ever touches `counter` (e.g. hoisting it into a register), and the hardware may
reorder or cache the value. The result is not just "sometimes wrong" — it is
unspecified by the standard.

## 3. The Fix

### 3.1 Use `std::atomic` for a single location

```cpp
#include <atomic>
#include <thread>
#include <vector>

std::atomic<int> counter{0};

void worker() {
    for (int i = 0; i < 100'000; ++i) {
        ++counter;  // atomic read-modify-write, well-defined
    }
}
```

### 3.2 Use a mutex when protecting a compound invariant

```cpp
#include <mutex>

int counter = 0;
std::mutex mtx;

void worker() {
    for (int i = 0; i < 100'000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;
    }
}
```

Use a mutex when the protected section involves more than a single atomic
variable — for example, transferring money between two balances, or updating a
counter together with a timestamp.

## 4. Detecting Data Races

Run your tests under **ThreadSanitizer** (supported by GCC and Clang):

```bash
g++ -fsanitize=thread -g -O1 main.cpp -o main
./main
```

ThreadSanitizer reports the exact pair of racing accesses with their stack
traces, which is far more reliable than trying to reproduce the bug by hand.

## 5. Best Practices

1. **Minimize shared mutable state** — pass data by value or use thread-local
   storage where possible.
2. **Prefer `std::atomic`** for individual counters, flags, and pointers.
3. **Use `std::mutex` (or `std::shared_mutex`)** for compound invariants.
4. **Test under ThreadSanitizer in CI** before declaring concurrency code safe.
5. **Document ownership** — which thread owns which data, and what is shared.

## 6. Summary

- A data race = unsynchronized concurrent access with at least one writer.
- It is **undefined behavior**, not merely "an occasional wrong answer".
- Fix with `std::atomic` (single location) or `std::mutex` (compound state).
- ThreadSanitizer is the fastest way to find races that testing cannot show.
