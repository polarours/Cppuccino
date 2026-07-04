#pragma once

#include <cstddef>
#include <memory>
#include <vector>

namespace simple_allocator {

class SimpleAllocator {
public:
    SimpleAllocator(std::size_t blockSize, std::size_t blockCount);
    ~SimpleAllocator();

    SimpleAllocator(const SimpleAllocator&) = delete;
    SimpleAllocator& operator=(const SimpleAllocator&) = delete;

    void* allocate(std::size_t size);
    void deallocate(void* ptr);

    std::size_t blockSize() const;
    std::size_t blockCount() const;
    std::size_t freeBlocks() const;
    std::size_t usedBlocks() const;

    bool owns(void* ptr) const;

    void reset();

private:
    struct Block {
        Block* next;
    };

    std::size_t blockSize_;
    std::size_t blockCount_;
    char* memory_;
    Block* freeList_;
    std::size_t usedCount_;
};

template <typename T>
class StlAllocator {
public:
    using value_type = T;

    explicit StlAllocator(SimpleAllocator& allocator) noexcept;
    template <typename U>
    StlAllocator(const StlAllocator<U>& other) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n);

    bool operator==(const StlAllocator& other) const noexcept;
    bool operator!=(const StlAllocator& other) const noexcept;

    template <typename U>
    friend class StlAllocator;

private:
    SimpleAllocator* allocator_;
};

// --- Template implementation ---

template <typename T>
StlAllocator<T>::StlAllocator(SimpleAllocator& allocator) noexcept
    : allocator_(&allocator) {}

template <typename T>
template <typename U>
StlAllocator<T>::StlAllocator(const StlAllocator<U>& other) noexcept
    : allocator_(other.allocator_) {}

template <typename T>
T* StlAllocator<T>::allocate(std::size_t n) {
    return static_cast<T*>(allocator_->allocate(n * sizeof(T)));
}

template <typename T>
void StlAllocator<T>::deallocate(T* p, std::size_t n) {
    allocator_->deallocate(p);
}

template <typename T>
bool StlAllocator<T>::operator==(const StlAllocator& other) const noexcept {
    return allocator_ == other.allocator_;
}

template <typename T>
bool StlAllocator<T>::operator!=(const StlAllocator& other) const noexcept {
    return allocator_ != other.allocator_;
}

} // namespace simple_allocator
