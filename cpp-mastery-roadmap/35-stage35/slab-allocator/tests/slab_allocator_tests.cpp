#include "slab_allocator.hpp"

#include <cstring>
#include <iostream>
#include <limits>
#include <new>
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

// Regression: a block that is bigger than the slab was handed a pointer
// into a buffer of the size of the slab, so writing the block wrote past
// the end of it. The address sanitizer found it.
void test_block_bigger_than_the_slab() {
    slab_allocator::SlabAllocator alloc(1024);

    void* p = alloc.allocate(4096);
    expect(p != nullptr, "expected a block");

    // The block has to be big enough for what was asked for. The
    // sanitizer checks that the memory behind it is really there.
    std::memset(p, 0xAB, 4096);
    const auto* bytes = static_cast<const unsigned char*>(p);
    expect(bytes[0] == 0xAB, "expected the first byte to be written");
    expect(bytes[4095] == 0xAB, "expected the last byte to be written");
}

// Regression: the loop that rounds a size up to a power of two shifted
// past the highest bit for a size that cannot be served, left zero
// behind and never ended.
void test_a_size_that_cannot_be_served() {
    slab_allocator::SlabAllocator alloc(1024);

    bool threw = false;
    try {
        alloc.allocate(std::numeric_limits<std::size_t>::max());
    } catch (const std::bad_alloc&) {
        threw = true;
    }
    expect(threw, "expected an allocation that cannot be served to throw");
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

        test_block_bigger_than_the_slab();
        std::cout << "  test_block_bigger_than_the_slab: PASS\n";

        test_a_size_that_cannot_be_served();
        std::cout << "  test_a_size_that_cannot_be_served: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "slab_allocator_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nslab_allocator_tests passed" << std::endl;
    return 0;
}
