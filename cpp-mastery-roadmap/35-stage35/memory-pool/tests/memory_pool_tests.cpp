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

// Regression: a pool of blocks of no bytes took the address of the first
// element of an empty pool, which is undefined. The undefined behaviour
// sanitizer found it.
void test_a_block_of_no_bytes_is_refused() {
    bool threw = false;
    try {
        memory_pool::MemoryPool pool(0, 10);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    expect(threw, "expected a pool of empty blocks to be refused");
}

// Regression: a block that was given back twice went onto the free list
// twice, and it was then handed out to two owners at the same time.
void test_a_block_that_is_freed_twice_is_refused() {
    memory_pool::MemoryPool pool(sizeof(int), 4);

    void* p = pool.allocate();
    pool.deallocate(p);
    pool.deallocate(p);

    expect(pool.freeBlocks() == 4, "expected 4 free blocks, got " +
                                       std::to_string(pool.freeBlocks()));

    void* first = pool.allocate();
    void* second = pool.allocate();
    expect(first != second, "expected two different blocks");
}

// A pointer into the middle of a block is inside the pool, but it is not
// the start of a block.
void test_a_pointer_into_a_block_is_refused() {
    memory_pool::MemoryPool pool(sizeof(int), 4);
    void* p = pool.allocate();
    auto* inside = static_cast<char*>(p) + 1;

    pool.deallocate(inside);
    expect(pool.freeBlocks() == 3, "expected the block to stay in use");
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

        test_a_block_of_no_bytes_is_refused();
        std::cout << "  test_a_block_of_no_bytes_is_refused: PASS\n";

        test_a_block_that_is_freed_twice_is_refused();
        std::cout << "  test_a_block_that_is_freed_twice_is_refused: PASS\n";

        test_a_pointer_into_a_block_is_refused();
        std::cout << "  test_a_pointer_into_a_block_is_refused: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "memory_pool_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nmemory_pool_tests passed" << std::endl;
    return 0;
}
