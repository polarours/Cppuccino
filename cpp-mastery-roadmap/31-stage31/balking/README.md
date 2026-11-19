# Balking Pattern

Skips action when preconditions are not met.

`isOpen()`/`isSaved()` read atomic state, so another thread can ask while
the transitions run under the mutex.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
