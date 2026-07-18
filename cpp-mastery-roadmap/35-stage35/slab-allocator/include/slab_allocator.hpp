#pragma once

#include <cstddef>
#include <vector>
#include <map>
#include <iostream>

namespace slab_allocator {

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

    std::size_t slabSize() const { return slabSize_; }

private:
    static std::size_t nextPowerOf2(std::size_t n) {
        std::size_t power = 1;
        while (power < n) power <<= 1;
        return power;
    }

    std::size_t slabSize_;
    std::size_t slabOffset_ = 0;
    std::vector<std::byte> currentSlab_;
    std::vector<std::vector<std::byte>> slabs_;
    std::map<std::size_t, std::vector<void*>> freeLists_;
};

} // namespace slab_allocator
