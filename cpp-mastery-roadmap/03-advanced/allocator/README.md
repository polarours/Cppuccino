# Simple Allocator

A simple fixed-block memory allocator with STL allocator interface.

## Learning Goals

- Understand memory allocation at a low level
- Practice RAII for memory management
- Learn how STL allocators work
- Implement a free list for memory reuse

## Non-Goals

- Building a production-grade allocator
- Supporting variable-size allocations
- Implementing garbage collection

## Features

- Fixed-block memory pool
- O(1) allocation and deallocation
- Free list for memory reuse
- STL-compatible allocator interface
- Memory ownership tracking

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/allocator_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "simple_allocator.hpp"

// Create allocator: 64-byte blocks, 10 blocks
simple_allocator::SimpleAllocator allocator(64, 10);

// Allocate and use
void* ptr = allocator.allocate(32);
// ... use ptr ...
allocator.deallocate(ptr);

// Use with STL containers
simple_allocator::StlAllocator<int> alloc(allocator);
std::vector<int, simple_allocator::StlAllocator<int>> vec(alloc);
vec.push_back(42);
```

## Suggested Next Steps

- Add variable-size allocation support
- Implement buddy allocator algorithm
- Add memory alignment support
- Add allocation statistics and debugging
