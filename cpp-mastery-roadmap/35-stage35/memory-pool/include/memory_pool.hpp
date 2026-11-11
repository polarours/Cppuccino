#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace memory_pool {

class MemoryPool {
public:
    explicit MemoryPool(std::size_t blockSize, std::size_t blockCount)
        : blockSize_(blockSize) {
        if (blockSize == 0) {
            // A block of no bytes is not a block, and the address of the
            // first element of an empty pool is not an address at all.
            throw std::invalid_argument("a pool needs blocks that are not "
                                        "empty");
        }
        if (blockCount > std::numeric_limits<std::size_t>::max() / blockSize) {
            // The multiplication below would wrap around and the pool
            // would be smaller than it says it is.
            throw std::bad_alloc();
        }
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

        auto* bytePtr = static_cast<std::byte*>(ptr);
        const std::size_t total = memory_.size();
        if (bytePtr < memory_.data() || bytePtr >= memory_.data() + total) {
            // The pointer is not inside the pool at all.
            return;
        }
        const std::size_t offset =
            static_cast<std::size_t>(bytePtr - memory_.data());
        if (offset % blockSize_ != 0) {
            // The pointer is inside a block, not the start of one.
            return;
        }
        if (std::find(freeList_.begin(), freeList_.end(), ptr) != freeList_.end()) {
            // The block is free already. Handing it out a second time
            // would give it to two owners at once.
            return;
        }
        freeList_.push_back(ptr);
    }

    std::size_t blockSize() const { return blockSize_; }
    std::size_t totalBlocks() const { return memory_.size() / blockSize_; }
    std::size_t freeBlocks() const { return freeList_.size(); }
    std::size_t usedBlocks() const { return totalBlocks() - freeBlocks(); }

private:
    std::size_t blockSize_;
    std::vector<std::byte> memory_;
    std::vector<void*> freeList_;
};

} // namespace memory_pool
