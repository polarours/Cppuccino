# Memory Pool & Allocators

## What Is a Memory Pool?

A memory pool pre-allocates fixed-size blocks so you avoid the overhead and
fragmentation of frequent `new`/`delete`. Allocation and deallocation are
O(1).

> **Core idea**: allocate one big chunk ("the pool") up front, keep the free
> blocks on a list or stack inside it, and hand them out on demand.

## When You Need One

- High-frequency allocate/free of same-sized objects (network packets, game
  entities, tree nodes)
- `new`/`delete` has become a bottleneck
- Memory allocation must be real-time (no system-call latency allowed)

## Three Allocators Compared

| | Memory Pool | Arena (Bump Allocator) | Slab Allocator |
|---|---|---|---|
| Block size | fixed | fixed | fixed |
| Free granularity | single block | whole-arena reset only | single block |
| Fragmentation | none | none | very low |
| Fit | many same-type objects created/destroyed | short-lived batch work | multiple size classes |

## Implementation: Memory Pool

```cpp
#include <cstddef>
#include <vector>
#include <cstdint>

class MemoryPool {
public:
    explicit MemoryPool(std::size_t blockSize, std::size_t blockCount)
        : blockSize_(blockSize) {
        memory_.resize(blockCount * blockSize);
        for (std::size_t i = 0; i < blockCount; ++i) {
            freeList_.push_back(&memory_[i * blockSize]);
        }
    }

    void* allocate() {
        if (freeList_.empty()) return nullptr;
        void* ptr = freeList_.back();
        freeList_.pop_back();
        return ptr;
    }

    void deallocate(void* ptr) {
        if (!ptr) return;
        freeList_.push_back(ptr);  // simple free list
    }

    std::size_t freeBlocks() const { return freeList_.size(); }
    std::size_t usedBlocks() const {
        return (memory_.size() / blockSize_) - freeList_.size();
    }

private:
    std::size_t blockSize_;
    std::vector<std::byte> memory_;
    std::vector<void*> freeList_;   // free list (a vector standing in)
};
```

## Implementation: Arena (Bump Allocator)

```cpp
#include <cstddef>
#include <vector>
#include <cstdint>

class Arena {
public:
    explicit Arena(std::size_t size) : memory_(size), offset_(0) {}

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) {
        std::size_t aligned = (offset_ + alignment - 1) & ~(alignment - 1);
        if (aligned + size > memory_.size()) return nullptr;
        void* ptr = &memory_[aligned];
        offset_ = aligned + size;
        return ptr;
    }

    void reset() { offset_ = 0; }  // free everything at once — no per-block tracking

    std::size_t used() const { return offset_; }

private:
    std::vector<std::byte> memory_;
    std::size_t offset_;   // the bump pointer
};
```

## Implementation: Slab Allocator

```cpp
#include <cstddef>
#include <vector>
#include <map>

class SlabAllocator {
public:
    explicit SlabAllocator(std::size_t slabSize = 4096) : slabSize_(slabSize) {
        currentSlab_.resize(slabSize);
    }

    void* allocate(std::size_t size) {
        std::size_t blockSize = nextPowerOf2(size);
        if (blockSize < 16) blockSize = 16;

        auto& freeList = freeLists_[blockSize];
        if (!freeList.empty()) {
            void* ptr = freeList.back();
            freeList.pop_back();
            return ptr;
        }

        if (slabOffset_ + blockSize > slabSize_) {
            slabs_.push_back(std::move(currentSlab_));
            currentSlab_.resize(slabSize_);
            slabOffset_ = 0;
        }

        void* ptr = &currentSlab_[slabOffset_];
        slabOffset_ += blockSize;
        return ptr;
    }

    void reset() {
        freeLists_.clear();
        slabs_.clear();
        currentSlab_.resize(slabSize_);
        slabOffset_ = 0;
    }

private:
    static std::size_t nextPowerOf2(std::size_t n) {
        std::size_t p = 1;
        while (p < n) p <<= 1;
        return p;
    }

    std::size_t slabSize_;
    std::size_t slabOffset_ = 0;
    std::vector<std::byte> currentSlab_;
    std::vector<std::vector<std::byte>> slabs_;
    std::map<std::size_t, std::vector<void*>> freeLists_;
};
```

For full runnable examples, see the
[memory-pool](../../cpp-mastery-roadmap/35-stage35/memory-pool),
[arena-allocator](../../cpp-mastery-roadmap/35-stage35/arena-allocator), and
[slab-allocator](../../cpp-mastery-roadmap/35-stage35/slab-allocator)
projects.

## Best Practices

1. **A pool serves one object shape**: never mix sizes or lifetimes in the
   same pool
2. **Arena `reset()` only at the right moment**: every object allocated from
   the arena must be dead first — typically batch-per-request or per-frame
3. **Always check the result**: all three allocators return `nullptr` when
   exhausted; callers must handle it
4. **Alignment is a silent constraint**: `Arena::allocate` takes an alignment
   argument defaulting to `alignof(std::max_align_t)`; requests smaller than
   natural alignment round to 8 or 16 bytes

## Pool vs Standard Allocators

| | new/delete | std::allocator | Memory pool |
|---|---|---|---|
| Allocation speed | slow (system call) | medium | fast (O(1) list pop) |
| Fragmentation | yes | medium | none |
| Scale | few / one-off | standard containers | huge numbers of same-shaped objects |

## Summary

A pool turns allocation cost from "system call" into "pointer pop" through
pre-allocation plus internal bookkeeping — standard infrastructure in game
engines, network servers, and high-frequency trading. The arena's bump-only
model is especially good for discard-as-a-batch workloads.
