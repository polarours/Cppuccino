# Memory Fragmentation

## Introduction

*Fragmentation* is what happens when free memory exists in total, but not as a single contiguous block large enough to satisfy an allocation. Over a long-running program that repeatedly allocates and frees *differently sized* blocks, the heap gets sliced into many small free "holes" separated by live objects. Eventually `new` fails (throws `std::bad_alloc` or returns `nullptr`) even though the summed free bytes would have been enough — they are just scattered.

Fragmentation is a *throughput and reliability* problem: it wastes RAM and can crash a server that has been "fine" for hours.

## The Problem

```cpp
#include <vector>

// Allocate many small, variable-size buffers, then free every *other* one.
void churn() {
    std::vector<int*> blocks;
    for (int i = 0; i < 1000; ++i) {
        blocks.push_back(new int[1 + (i % 8)]);   // sizes 1..8 ints, interleaved
    }
    for (size_t i = 0; i < blocks.size(); i += 2) {
        delete[] blocks[i];                       // frees odd indices, leaves even
        blocks[i] = nullptr;
    }
    // Heap now has 500 free holes of varying sizes, interleaved with 500 live ones.
    // A later request for a *large* contiguous block may fail despite free RAM.
}
```

**Why it's broken:**
- The default `::operator new` allocates from a general-purpose heap. Mixed lifetimes and sizes scatter free space.
- Freeing every other block leaves live blocks wedged between freed holes, so no hole is large enough for a big allocation — classic external fragmentation.
- Repeated `new`/`delete` of tiny objects also stresses the allocator and worsens fragmentation over time.

This pattern is common in parsers, request handlers, and game loops that build and tear down many mixed-size objects per tick.

## The Fix

Allocate from a region sized for your *actual* working set, using a **pool / arena allocator**, so every block comes from one contiguous buffer and frees in bulk:

```cpp
#include <memory>
#include <memory_resource>
#include <vector>

// C++17 polymorphic allocator: a monotonic buffer hands out memory with zero
// per-allocation overhead and NO fragmentation, because nothing is individually
// freed — the whole buffer is released at once.
void churn() {
    std::vector<char> arena(64 * 1024);                    // one contiguous region
    std::pmr::monotonic_buffer_resource pool(arena.data(), arena.size());
    std::pmr::vector<int> blocks(&pool);                   // backed by the pool

    for (int i = 0; i < 1000; ++i) {
        blocks.resize(blocks.size() + 1 + (i % 8));        // grows inside the arena
    }
    // No per-element free -> no fragmentation. When `arena` goes out of scope,
    // everything is reclaimed in O(1).
}
```

For object pools of a *fixed* type, a `std::vector<T>` (contiguous storage) or a free-list pool of reused `T` instances avoids the heap entirely between allocations.

## Best Practices

- **Group allocations by lifetime.** Allocate everything for one "tick"/request from one arena, then free the arena at once. Bulk free eliminates fragmentation.
- **Prefer contiguous containers.** `std::vector<T>` (not `std::vector<T*>` with per-element `new`) keeps data in one block and is cache-friendly too.
- **Use `std::pmr` for hot paths.** `std::pmr::monotonic_buffer_resource` / `std::pmr::unsynchronized_pool_resource` (C++17) give you pool/arena behavior with almost no code.
- **Reuse objects.** A free-list or object pool recycles freed instances instead of returning them to the general heap — great for fixed-size structures.
- **Measure, don't guess.** Track peak RSS and allocator stats under realistic load; fragmentation shows up only at scale, so test with production-sized churn.

## Summary

Fragmentation is the silent tax of mixing many differently sized, differently timed allocations on a general-purpose heap. You cannot "fix" the heap; you change *how you allocate*: give each phase of work its own arena and reclaim it in bulk, keep homogeneous data contiguous in `std::vector`, and reach for `std::pmr` pools on hot paths. Do that, and the "out of memory despite free RAM" crash disappears.
