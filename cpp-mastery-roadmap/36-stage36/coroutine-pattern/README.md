# Coroutine Pattern

C++20 coroutines for lazy evaluation and generators.

A generator can be moved into another variable, and moved over one that
already holds a generator: the coroutine that was there is destroyed, so
it is not left behind. A generator that was moved from holds nothing, and
says so when it is asked for its value.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
