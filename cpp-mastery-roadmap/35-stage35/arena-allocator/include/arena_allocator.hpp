#pragma once

#include <cstddef>
#include <vector>
#include <iostream>

namespace arena_allocator {

class Arena {
public:
    explicit Arena(std::size_t size) : memory_(size), offset_(0) {}

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) {
        std::size_t alignedOffset = (offset_ + alignment - 1) & ~(alignment - 1);
        // alignedOffset + size can wrap around on a huge request and slip
        // past the check, so subtract instead of adding.
        if (alignedOffset > memory_.size() || size > memory_.size() - alignedOffset)
            return nullptr;
        void* ptr = &memory_[alignedOffset];
        offset_ = alignedOffset + size;
        return ptr;
    }

    void reset() { offset_ = 0; }

    std::size_t used() const { return offset_; }
    std::size_t capacity() const { return memory_.size(); }
    std::size_t remaining() const { return capacity() - used(); }

private:
    std::vector<std::byte> memory_;
    std::size_t offset_;
};

} // namespace arena_allocator
