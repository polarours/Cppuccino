# Guarded Suspension

Waits for a condition before proceeding with thread-safe access.

## Thread Safety

The state of the object is atomic, so reading it needs no lock. That is
what lets the predicate of `waitFor()` ask the object about itself:

```cpp
#include "guarded_suspension.hpp"

guarded_suspension::GuardedObject obj;

obj.waitFor([&obj]() { return obj.isReady(); });
```

A condition variable calls the predicate of its wait **with the lock
held**, so a predicate that needed the lock would wait for a lock the
thread already has. The mutex is there for the writers and for the
condition variable itself.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
