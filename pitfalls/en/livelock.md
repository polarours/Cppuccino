# Livelock

## 1. Introduction

A **livelock** is a situation where threads (or processes) keep changing state
in response to each other but make **no overall progress**. Unlike a deadlock,
the threads are *running* — they are busy and consuming CPU — yet the actual
work never completes.

Livelock is sometimes described as "two polite people who keep stepping aside to
let the other pass, and end up never moving."

## 2. The Problem

A common cause is overly "polite" collision handling with retries:

```cpp
#include <atomic>
#include <thread>

std::atomic<bool> turn{false};  // whose "turn" is it to proceed?

void worker_a() {
    while (true) {
        if (!turn.load()) {
            turn.store(true);            // politely give B a turn
            std::this_thread::yield();
            continue;                    // retry
        }
        do_work();
        turn.store(false);
        break;
    }
}

void worker_b() {
    while (true) {
        if (turn.load()) {
            turn.store(false);           // politely give A a turn
            std::this_thread::yield();
            continue;
        }
        do_work();
        turn.store(true);
        break;
    }
}
```

### Why it's broken

Both threads perpetually detect a "conflict", yield, and hand the turn to the
other. Each retry resets the other's progress. They spin forever, burning CPU,
but neither ever reaches `do_work()`. The threads are alive and scheduled, yet
the system makes zero progress.

## 3. The Fix

### 3.1 Randomized backoff

Introduce randomness so the threads do not keep synchronizing their retries:

```cpp
#include <chrono>
#include <random>

void worker() {
    std::mt19937 rng{std::random_device{}()};
    while (true) {
        if (try_acquire()) { do_work(); break; }
        // exponential + random backoff breaks the symmetry
        auto ms = std::uniform_int_distribution<int>(1, 10)(rng);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}
```

### 3.2 Single coordinator / clear ownership

Assign one thread to make decisions, or use a lock-free queue with a clear owner,
so there is no symmetric "polite" dance to begin with.

### 3.3 Use proper synchronization primitives

Often a `std::mutex` or condition variable removes the need for hand-rolled
yield-and-retry logic entirely.

## 4. Best Practices

1. **Never busy-wait with only `yield()`** — add real backoff or blocking.
2. **Randomize retry timing** to break symmetry between contending threads.
3. **Prefer mutex / condition_variable** over manual retry loops.
4. **Watch CPU usage** — 100% busy but no output is a livelock smell.

## 5. Summary

- Livelock = threads run but make no progress (vs. deadlock = threads blocked).
- Usually caused by symmetric, over-polite retry / yield logic.
- Fix with randomized backoff, single ownership, or proper primitives.
- High CPU usage with no results is the key symptom.
