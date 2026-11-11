# Memory Pool

Pre-allocated fixed-size block pool.

A block that is given back twice is refused: handing it out again would
give the same memory to two owners at once.

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
