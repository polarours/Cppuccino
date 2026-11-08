# Memory Pool

Pre-allocated fixed-size block pool.

## Block Sizes

A block of no bytes is not a block: a pool that is asked for one throws
`std::invalid_argument`. A pool whose size would not fit in memory
throws `std::bad_alloc`.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
