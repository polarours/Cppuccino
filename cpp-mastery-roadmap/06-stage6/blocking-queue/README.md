# Blocking Queue

Bounded MPMC queue: two condition variables (not-empty / not-full),
timeout-aware push/pop, and close() that wakes all waiters.

## Learning Goals

- Why one mutex needs two condition variables for a bounded queue
- Predicate-based wait_for to avoid spurious wakeups
- close() semantics: drain-then-nullopt, push fails
- Proving "consumed exactly once" in a producer/consumer test

## Non-Goals

- Lock-free implementations (see stage 33 lock-free-queue)
- Priority ordering
- Per-consumer fairness

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
