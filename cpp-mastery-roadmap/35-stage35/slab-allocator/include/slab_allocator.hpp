#pragma once

#include <cstddef>
#include <vector>
#include <map>
#include <iostream>
#include <limits>
#include <new>

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

        // A slab has to be at least as big as the block it holds. A
        // block that does not fit in a slab of the usual size gets a slab
        // of its own: handing out a pointer into a buffer that is too
        // small means that the caller writes past the end of it.
        if (blockSize > slabSize_) {
            slabs_.push_back(std::vector<std::byte>(blockSize));
            return slabs_.back().data();
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
        while (power < n) {
            // Shifting past the highest bit would leave zero behind, and
            // the loop would never end.
            if (power > std::numeric_limits<std::size_t>::max() / 2) {
                throw std::bad_alloc();
            }
            power <<= 1;
        }
        return power;
    }

    std::size_t slabSize_;
    std::size_t slabOffset_ = 0;
    std::vector<std::byte> currentSlab_;
    std::vector<std::vector<std::byte>> slabs_;
    std::map<std::size_t, std::vector<void*>> freeLists_;
};

} // namespace slab_allocator
