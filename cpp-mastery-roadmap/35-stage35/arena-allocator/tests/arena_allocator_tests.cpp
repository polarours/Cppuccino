#include "arena_allocator.hpp"

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
    arena_allocator::Arena arena(1024);

    void* p1 = arena.allocate(100);
    expect(p1 != nullptr, "expected non-null");
    expect(arena.used() >= 100, "expected at least 100 used");
}

void test_multiple_alloc() {
    arena_allocator::Arena arena(1024);

    arena.allocate(100);
    arena.allocate(200);
    arena.allocate(300);

    expect(arena.used() >= 600, "expected at least 600 used");
    expect(arena.remaining() < 500, "expected less than 500 remaining");
}

void test_reset() {
    arena_allocator::Arena arena(1024);

    arena.allocate(100);
    arena.allocate(200);
    arena.reset();

    expect(arena.used() == 0, "expected 0 after reset");
    expect(arena.remaining() == 1024, "expected full capacity");
}

void test_oom() {
    arena_allocator::Arena arena(100);

    void* p1 = arena.allocate(100);
    expect(p1 != nullptr, "expected first alloc");

    void* p2 = arena.allocate(100);
    expect(p2 == nullptr, "expected null when full");
}

} // namespace

int main() {
    try {
        std::cout << "Arena Allocator Tests:\n";
        test_basic_alloc();
        std::cout << "  test_basic_alloc: PASS\n";

        test_multiple_alloc();
        std::cout << "  test_multiple_alloc: PASS\n";

        test_reset();
        std::cout << "  test_reset: PASS\n";

        test_oom();
        std::cout << "  test_oom: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "arena_allocator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\narena_allocator_tests passed" << std::endl;
    return 0;
}
