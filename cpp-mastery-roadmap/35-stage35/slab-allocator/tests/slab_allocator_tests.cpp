#include "slab_allocator.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_alloc() {
    slab_allocator::SlabAllocator alloc(1024);

    void* p1 = alloc.allocate(32);
    void* p2 = alloc.allocate(64);

    expect(p1 != nullptr, "expected non-null");
    expect(p2 != nullptr, "expected non-null");
}

void test_power_of_2_sizes() {
    slab_allocator::SlabAllocator alloc(1024);

    alloc.allocate(3);   // becomes 16
    alloc.allocate(17);  // becomes 32
    alloc.allocate(65);  // becomes 128

    expect(alloc.slabSize() == 1024, "expected slab size 1024");
}

void test_multiple_allocs() {
    slab_allocator::SlabAllocator alloc(512);

    for (int i = 0; i < 10; ++i) {
        void* p = alloc.allocate(16);
        expect(p != nullptr, "expected non-null alloc");
    }
}

void test_reset() {
    slab_allocator::SlabAllocator alloc(256);
    alloc.allocate(32);
    alloc.allocate(64);
    alloc.reset();

    void* p = alloc.allocate(16);
    expect(p != nullptr, "expected alloc after reset");
}

} // namespace

int main() {
    try {
        std::cout << "Slab Allocator Tests:\n";
        test_basic_alloc();
        std::cout << "  test_basic_alloc: PASS\n";

        test_power_of_2_sizes();
        std::cout << "  test_power_of_2_sizes: PASS\n";

        test_multiple_allocs();
        std::cout << "  test_multiple_allocs: PASS\n";

        test_reset();
        std::cout << "  test_reset: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "slab_allocator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nslab_allocator_tests passed" << std::endl;
    return 0;
}
