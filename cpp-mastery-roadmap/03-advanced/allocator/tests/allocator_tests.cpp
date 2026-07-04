#include "simple_allocator.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_alloc_dealloc() {
    simple_allocator::SimpleAllocator allocator(64, 10);

    void* ptr1 = allocator.allocate(32);
    expect(ptr1 != nullptr, "expected non-null pointer");

    void* ptr2 = allocator.allocate(32);
    expect(ptr2 != nullptr, "expected non-null pointer");
    expect(ptr1 != ptr2, "expected different pointers");

    allocator.deallocate(ptr1);
    allocator.deallocate(ptr2);
}

void test_alloc_returns_free_blocks() {
    simple_allocator::SimpleAllocator allocator(64, 4);

    expect(allocator.freeBlocks() == 4, "expected 4 free blocks");
    expect(allocator.usedBlocks() == 0, "expected 0 used blocks");

    void* ptr1 = allocator.allocate(32);
    expect(allocator.freeBlocks() == 3, "expected 3 free blocks");
    expect(allocator.usedBlocks() == 1, "expected 1 used block");

    void* ptr2 = allocator.allocate(32);
    expect(allocator.freeBlocks() == 2, "expected 2 free blocks");
    expect(allocator.usedBlocks() == 2, "expected 2 used blocks");

    allocator.deallocate(ptr1);
    expect(allocator.freeBlocks() == 3, "expected 3 free blocks after dealloc");
    expect(allocator.usedBlocks() == 1, "expected 1 used block after dealloc");

    allocator.deallocate(ptr2);
}

void test_owns_pointer() {
    simple_allocator::SimpleAllocator allocator(64, 4);

    void* ptr = allocator.allocate(32);
    expect(allocator.owns(ptr), "expected allocator to own pointer");

    allocator.deallocate(ptr);
    expect(allocator.owns(ptr), "expected allocator to still own freed pointer");

    int stack_var = 42;
    expect(!allocator.owns(&stack_var), "expected allocator not to own stack variable");
}

void test_exhaust_pool() {
    simple_allocator::SimpleAllocator allocator(64, 2);

    void* ptr1 = allocator.allocate(32);
    void* ptr2 = allocator.allocate(32);
    expect(ptr1 != nullptr && ptr2 != nullptr, "expected two allocations");

    void* ptr3 = allocator.allocate(32);
    expect(ptr3 == nullptr, "expected null when pool exhausted");

    allocator.deallocate(ptr1);
    allocator.deallocate(ptr2);
}

void test_reset() {
    simple_allocator::SimpleAllocator allocator(64, 4);

    allocator.allocate(32);
    allocator.allocate(32);
    expect(allocator.usedBlocks() == 2, "expected 2 used blocks");

    allocator.reset();
    expect(allocator.usedBlocks() == 0, "expected 0 used blocks after reset");
    expect(allocator.freeBlocks() == 4, "expected 4 free blocks after reset");
}

void test_dealloc_null() {
    simple_allocator::SimpleAllocator allocator(64, 4);
    allocator.deallocate(nullptr);  // Should not crash
    expect(allocator.freeBlocks() == 4, "expected 4 free blocks");
}

void test_stl_allocator() {
    simple_allocator::SimpleAllocator allocator(sizeof(int) * 10, 10);
    simple_allocator::StlAllocator<int> alloc(allocator);

    std::vector<int, simple_allocator::StlAllocator<int>> vec(alloc);

    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 10);
    }

    expect(vec.size() == 5, "expected 5 elements");
    expect(vec[0] == 0, "expected first element 0");
    expect(vec[4] == 40, "expected last element 40");
}

void test_stl_list() {
    simple_allocator::SimpleAllocator allocator(sizeof(int) * 20, 20);
    simple_allocator::StlAllocator<int> alloc(allocator);

    std::list<int, simple_allocator::StlAllocator<int>> lst(alloc);

    for (int i = 0; i < 10; ++i) {
        lst.push_back(i);
    }

    expect(lst.size() == 10, "expected 10 elements");

    int sum = 0;
    for (int val : lst) {
        sum += val;
    }
    expect(sum == 45, "expected sum 45");
}

} // namespace

int main() {
    try {
        std::cout << "SimpleAllocator Tests:\n";
        test_basic_alloc_dealloc();
        std::cout << "  test_basic_alloc_dealloc: PASS\n";

        test_alloc_returns_free_blocks();
        std::cout << "  test_alloc_returns_free_blocks: PASS\n";

        test_owns_pointer();
        std::cout << "  test_owns_pointer: PASS\n";

        test_exhaust_pool();
        std::cout << "  test_exhaust_pool: PASS\n";

        test_reset();
        std::cout << "  test_reset: PASS\n";

        test_dealloc_null();
        std::cout << "  test_dealloc_null: PASS\n";

        test_stl_allocator();
        std::cout << "  test_stl_allocator: PASS\n";

        test_stl_list();
        std::cout << "  test_stl_list: PASS\n";
    } catch (const std::exception& exception) {
        std::cerr << "allocator_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nallocator_tests passed" << std::endl;
    return 0;
}
