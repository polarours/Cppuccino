// examples/memory-pool-demo.cpp
// Demonstrates three ways of handing out memory: a free list pool, an arena that never gives anything back, and a slab allocator with one free list per size.
// Compile with: g++ -std=c++17 -o memory_pool_demo memory-pool-demo.cpp

#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <new>
#include <string>
#include <vector>

// --- 1. A pool of fixed size blocks, kept in a free list ---

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
        if (freeList_.empty()) {
            return nullptr;
        }
        void* ptr = freeList_.back();
        freeList_.pop_back();
        return ptr;
    }

    void deallocate(void* ptr) {
        if (!ptr) {
            return;
        }
        freeList_.push_back(ptr);
    }

    std::size_t freeBlocks() const { return freeList_.size(); }

    std::size_t usedBlocks() const {
        return (memory_.size() / blockSize_) - freeList_.size();
    }

private:
    std::size_t blockSize_;
    std::vector<std::byte> memory_;
    std::vector<void*> freeList_;
};

// --- 2. An arena: it hands out memory and never gives it back ---

class Arena {
public:
    explicit Arena(std::size_t chunkSize = 4096) : chunkSize_(chunkSize) {}

    void* allocate(std::size_t size) {
        std::size_t aligned = (size + alignof(std::max_align_t) - 1) &
                              ~(alignof(std::max_align_t) - 1);

        if (chunks_.empty() || offset_ + aligned > chunks_.back().size()) {
            chunks_.emplace_back(chunkSize_ + aligned, std::byte{});
            offset_ = 0;
        }

        void* ptr = chunks_.back().data() + offset_;
        offset_ += aligned;
        return ptr;
    }

    std::size_t bytesUsed() const {
        std::size_t total = 0;
        for (const std::vector<std::byte>& chunk : chunks_) {
            total += chunk.size();
        }
        return total;
    }

private:
    std::size_t chunkSize_;
    std::vector<std::vector<std::byte>> chunks_;
    std::size_t offset_ = 0;
};

// --- 3. A slab allocator: one free list per size class ---

class SlabAllocator {
public:
    explicit SlabAllocator(std::size_t slabSize = 4096) : slabSize_(slabSize) {
        currentSlab_.resize(slabSize);
    }

    void* allocate(std::size_t size) {
        std::size_t blockSize = nextPowerOf2(size);
        if (blockSize < 16) {
            blockSize = 16;
        }

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

    void deallocate(void* ptr, std::size_t size) {
        std::size_t blockSize = nextPowerOf2(size);
        if (blockSize < 16) {
            blockSize = 16;
        }
        freeLists_[blockSize].push_back(ptr);
    }

    std::size_t slabCount() const { return slabs_.size() + 1; }

private:
    static std::size_t nextPowerOf2(std::size_t value) {
        std::size_t result = 1;
        while (result < value) {
            result <<= 1;
        }
        return result;
    }

    std::size_t slabSize_;
    std::vector<std::byte> currentSlab_;
    std::vector<std::vector<std::byte>> slabs_;
    std::size_t slabOffset_ = 0;
    std::map<std::size_t, std::vector<void*>> freeLists_;
};

// --- 4. What they are good for ---

int main() {
    std::cout << "=== Memory Pool Demo ===\n\n";

    std::cout << "--- 1. A pool of fixed size blocks ---\n";
    MemoryPool pool(64, 4);
    std::cout << "  free blocks at the start: " << pool.freeBlocks() << "\n";

    void* first = pool.allocate();
    void* second = pool.allocate();
    std::cout << "  free blocks after two allocations: " << pool.freeBlocks() << "\n";

    pool.deallocate(first);
    std::cout << "  free blocks after giving one back: " << pool.freeBlocks() << "\n";

    // The pool runs out rather than growing.
    void* third = pool.allocate();
    void* fourth = pool.allocate();
    void* fifth = pool.allocate();
    std::cout << "  a fifth allocation: " << (fifth == nullptr ? "nothing left"
                                                             : "one more")
              << "\n";

    pool.deallocate(second);
    pool.deallocate(third);
    pool.deallocate(fourth);
    std::cout << "  free blocks at the end: " << pool.freeBlocks() << "\n";

    std::cout << "\n--- 2. An arena: it never gives anything back ---\n";
    Arena arena(256);
    for (int i = 0; i < 10; ++i) {
        char* text = static_cast<char*>(arena.allocate(32));
        std::strcpy(text, "a short message");
    }
    std::cout << "  ten messages of 32 bytes, and nothing was freed\n";
    std::cout << "  the arena holds " << arena.bytesUsed() << " bytes\n";
    std::cout << "  that is the trade: freeing is free, and the memory is\n"
                 "  only released when the whole arena is\n";

    std::cout << "\n--- 3. A slab allocator: one free list per size ---\n";
    SlabAllocator slabs(512);
    for (int i = 0; i < 20; ++i) {
        slabs.allocate(24);
    }
    for (int i = 0; i < 20; ++i) {
        slabs.allocate(200);
    }
    std::cout << "  slabs after twenty small and twenty large allocations: "
              << slabs.slabCount() << "\n";
    std::cout << "  the small ones and the large ones live in different\n"
                 "  size classes, so one does not waste the other\n";

    std::cout << "\n=== All allocator scenarios completed ===\n";
    return 0;
}
