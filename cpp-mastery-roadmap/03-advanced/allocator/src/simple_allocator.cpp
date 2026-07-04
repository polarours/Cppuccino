#include "simple_allocator.hpp"

#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace simple_allocator {

SimpleAllocator::SimpleAllocator(std::size_t blockSize, std::size_t blockCount)
    : blockSize_(std::max(blockSize, sizeof(Block)))
    , blockCount_(blockCount)
    , memory_(new char[blockSize_ * blockCount_])
    , freeList_(nullptr)
    , usedCount_(0)
{
    reset();
}

SimpleAllocator::~SimpleAllocator() {
    delete[] memory_;
}

void* SimpleAllocator::allocate(std::size_t size) {
    if (size > blockSize_) {
        return nullptr;
    }

    if (!freeList_) {
        return nullptr;
    }

    Block* block = freeList_;
    freeList_ = block->next;
    usedCount_++;

    return static_cast<void*>(block);
}

void SimpleAllocator::deallocate(void* ptr) {
    if (!ptr) {
        return;
    }

    if (!owns(ptr)) {
        return;
    }

    Block* block = static_cast<Block*>(ptr);
    block->next = freeList_;
    freeList_ = block;
    usedCount_--;
}

std::size_t SimpleAllocator::blockSize() const {
    return blockSize_;
}

std::size_t SimpleAllocator::blockCount() const {
    return blockCount_;
}

std::size_t SimpleAllocator::freeBlocks() const {
    std::size_t count = 0;
    Block* current = freeList_;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

std::size_t SimpleAllocator::usedBlocks() const {
    return usedCount_;
}

bool SimpleAllocator::owns(void* ptr) const {
    char* p = static_cast<char*>(ptr);
    return p >= memory_ && p < memory_ + blockSize_ * blockCount_;
}

void SimpleAllocator::reset() {
    freeList_ = reinterpret_cast<Block*>(memory_);
    Block* current = freeList_;

    for (std::size_t i = 1; i < blockCount_; ++i) {
        current->next = reinterpret_cast<Block*>(memory_ + i * blockSize_);
        current = current->next;
    }
    current->next = nullptr;

    usedCount_ = 0;
}

} // namespace simple_allocator
