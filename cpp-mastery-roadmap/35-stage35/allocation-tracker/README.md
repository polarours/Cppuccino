# Allocation Tracker

Process-wide allocation counters (live/peak/failures) plus a stateless
counting allocator for std containers, and a new_handler hook.

## Learning Goals

- Atomic counters: relaxed ordering is enough for statistics
- Peak tracking via compare_exchange loop
- STL allocator requirements: rebind equality for stateless allocators
- new_handler contract: throw bad_alloc or free memory and retry

## Non-Goals

- Replacing global operator new (leaves byte-accurate leak tracking to ASan)
- Allocation sampling / flame graphs
- Cross-thread attribution

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
