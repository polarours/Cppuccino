#pragma once

#include <cstddef>
#include <new>
#include <iostream>

namespace memory_alignment {

struct alignas(64) CacheLineAligned {
    int data[16];
    void print() const { std::cout << "CacheLineAligned at " << this << "\n"; }
};

struct alignas(32) CacheAligned32 {
    int data[8];
};

template <typename T>
class AlignedAllocator {
public:
    using value_type = T;

    AlignedAllocator() = default;

    template <typename U>
    AlignedAllocator(const AlignedAllocator<U>&) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T), std::align_val_t{alignof(T)}));
    }

    void deallocate(T* p, std::size_t) {
        ::operator delete(p, std::align_val_t{alignof(T)});
    }
};

template <typename T, std::size_t Alignment = alignof(T)>
struct alignas(Alignment) AlignedStorage {
    T value;

    T& get() { return value; }
    const T& get() const { return value; }
};

class PaddedCounter {
public:
    void increment() { ++count_; }
    int count() const { return count_; }

private:
    alignas(64) int count_ = 0;
    char padding_[64 - sizeof(int)];
};

class UnpaddedCounter {
public:
    void increment() { ++count_; }
    int count() const { return count_; }

private:
    int count_ = 0;
};

} // namespace memory_alignment
