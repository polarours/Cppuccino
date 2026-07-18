#pragma once

#include <cstddef>
#include <vector>
#include <iostream>

namespace memory_pool {

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
        auto* bytePtr = static_cast<std::byte*>(ptr);
        auto offset = bytePtr - memory_.data();
        if (offset >= 0 && static_cast<std::size_t>(offset) < memory_.size()) {
            freeList_.push_back(ptr);
        }
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
