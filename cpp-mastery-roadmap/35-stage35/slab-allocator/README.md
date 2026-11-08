# Slab Allocator

Size-classified memory cache.

## Block Sizes

The size of a block is the request rounded up to a power of two, and at
least 16 bytes. A block that is bigger than the slab gets a slab of its
own, so that the memory behind the pointer really is as big as the block.

A request that cannot be served at all throws `std::bad_alloc` instead
of rounding the size up forever.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
