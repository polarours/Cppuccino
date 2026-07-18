#include "memory_pool.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_alloc_dealloc() {
    memory_pool::MemoryPool pool(sizeof(int), 4);

    void* p1 = pool.allocate();
    expect(p1 != nullptr, "expected non-null");
    expect(pool.freeBlocks() == 3, "expected 3 free");

    pool.deallocate(p1);
    expect(pool.freeBlocks() == 4, "expected 4 free after dealloc");
}

void test_fill_pool() {
    memory_pool::MemoryPool pool(sizeof(int), 2);

    void* p1 = pool.allocate();
    void* p2 = pool.allocate();
    void* p3 = pool.allocate();

    expect(p1 != nullptr && p2 != nullptr, "expected 2 allocs");
    expect(p3 == nullptr, "expected null when full");
    expect(pool.usedBlocks() == 2, "expected 2 used");
}

void test_reuse_memory() {
    memory_pool::MemoryPool pool(sizeof(double), 2);

    void* p1 = pool.allocate();
    pool.deallocate(p1);
    void* p2 = pool.allocate();

    expect(p2 != nullptr, "expected reuse");
    expect(pool.freeBlocks() == 1, "expected 1 free");
}

void test_block_size() {
    memory_pool::MemoryPool pool(64, 10);
    expect(pool.blockSize() == 64, "expected 64");
    expect(pool.totalBlocks() == 10, "expected 10");
}

} // namespace

int main() {
    try {
        std::cout << "Memory Pool Tests:\n";
        test_basic_alloc_dealloc();
        std::cout << "  test_basic_alloc_dealloc: PASS\n";

        test_fill_pool();
        std::cout << "  test_fill_pool: PASS\n";

        test_reuse_memory();
        std::cout << "  test_reuse_memory: PASS\n";

        test_block_size();
        std::cout << "  test_block_size: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "memory_pool_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nmemory_pool_tests passed" << std::endl;
    return 0;
}
