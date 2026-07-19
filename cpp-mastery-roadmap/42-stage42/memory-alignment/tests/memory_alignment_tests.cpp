#include "memory_alignment.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_alignment() {
    memory_alignment::CacheLineAligned obj;
    auto addr = reinterpret_cast<std::uintptr_t>(&obj);
    expect(addr % 64 == 0, "expected 64-byte alignment");
}

void test_32_byte_alignment() {
    memory_alignment::CacheAligned32 obj;
    auto addr = reinterpret_cast<std::uintptr_t>(&obj);
    expect(addr % 32 == 0, "expected 32-byte alignment");
}

void test_aligned_storage() {
    memory_alignment::AlignedStorage<int, 64> storage;
    storage.get() = 42;

    auto addr = reinterpret_cast<std::uintptr_t>(&storage);
    expect(addr % 64 == 0, "expected 64-byte alignment");
    expect(storage.get() == 42, "expected 42");
}

void test_padded_counter() {
    memory_alignment::PaddedCounter counter;
    counter.increment();
    counter.increment();
    expect(counter.count() == 2, "expected 2");
}

void test_aligned_allocator() {
    memory_alignment::AlignedAllocator<int> alloc;
    int* p = alloc.allocate(1);
    *p = 100;
    expect(*p == 100, "expected 100");
    alloc.deallocate(p, 1);
}

} // namespace

int main() {
    try {
        std::cout << "Memory Alignment Tests:\n";
        test_alignment();
        std::cout << "  test_alignment: PASS\n";

        test_32_byte_alignment();
        std::cout << "  test_32_byte_alignment: PASS\n";

        test_aligned_storage();
        std::cout << "  test_aligned_storage: PASS\n";

        test_padded_counter();
        std::cout << "  test_padded_counter: PASS\n";

        test_aligned_allocator();
        std::cout << "  test_aligned_allocator: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "memory_alignment_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nmemory_alignment_tests passed" << std::endl;
    return 0;
}
