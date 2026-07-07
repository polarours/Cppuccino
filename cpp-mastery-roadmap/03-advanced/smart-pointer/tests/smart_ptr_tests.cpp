#include "smart_ptr.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// --- UniquePtr tests ---

void test_unique_ptr_basic() {
    smart_ptr::UniquePtr<int> ptr(new int(42));
    expect(*ptr == 42, "expected 42");
    expect(ptr.get() != nullptr, "expected non-null");
}

void test_unique_ptr_move() {
    smart_ptr::UniquePtr<int> ptr1(new int(10));
    smart_ptr::UniquePtr<int> ptr2 = std::move(ptr1);

    expect(*ptr2 == 10, "expected 10");
    expect(ptr1 == nullptr, "expected nullptr after move");
}

void test_unique_ptr_release() {
    smart_ptr::UniquePtr<int> ptr(new int(20));
    int* raw = ptr.release();

    expect(*raw == 20, "expected 20");
    expect(ptr == nullptr, "expected nullptr after release");
    delete raw;
}

void test_unique_ptr_reset() {
    smart_ptr::UniquePtr<int> ptr(new int(30));
    ptr.reset(new int(40));

    expect(*ptr == 40, "expected 40");
}

void test_unique_ptr_bool() {
    smart_ptr::UniquePtr<int> ptr1(new int(1));
    smart_ptr::UniquePtr<int> ptr2;

    expect(static_cast<bool>(ptr1), "expected true for non-null");
    expect(!static_cast<bool>(ptr2), "expected false for null");
}

// --- SharedPtr tests ---

void test_shared_ptr_basic() {
    smart_ptr::SharedPtr<int> ptr(new int(42));
    expect(*ptr == 42, "expected 42");
    expect(ptr.use_count() == 1, "expected use_count 1");
}

void test_shared_ptr_copy() {
    smart_ptr::SharedPtr<int> ptr1(new int(10));
    smart_ptr::SharedPtr<int> ptr2 = ptr1;

    expect(*ptr2 == 10, "expected 10");
    expect(ptr1.use_count() == 2, "expected use_count 2");
    expect(ptr2.use_count() == 2, "expected use_count 2");
}

void test_shared_ptr_move() {
    smart_ptr::SharedPtr<int> ptr1(new int(20));
    smart_ptr::SharedPtr<int> ptr2 = std::move(ptr1);

    expect(*ptr2 == 20, "expected 20");
    expect(ptr1 == nullptr, "expected nullptr after move");
    expect(ptr2.use_count() == 1, "expected use_count 1");
}

void test_shared_ptr_assignment() {
    smart_ptr::SharedPtr<int> ptr1(new int(30));
    smart_ptr::SharedPtr<int> ptr2(new int(40));

    ptr1 = ptr2;
    expect(*ptr1 == 40, "expected 40");
    expect(ptr1.use_count() == 2, "expected use_count 2");
}

void test_shared_ptr_reset() {
    smart_ptr::SharedPtr<int> ptr1(new int(50));
    smart_ptr::SharedPtr<int> ptr2 = ptr1;

    ptr1.reset();
    expect(ptr1 == nullptr, "expected nullptr after reset");
    expect(ptr2.use_count() == 1, "expected use_count 1");
}

void test_shared_ptr_unique() {
    smart_ptr::SharedPtr<int> ptr1(new int(60));
    expect(ptr1.unique(), "expected unique");

    smart_ptr::SharedPtr<int> ptr2 = ptr1;
    expect(!ptr1.unique(), "expected not unique");
}

// --- WeakPtr tests ---

void test_weak_ptr_basic() {
    smart_ptr::SharedPtr<int> shared(new int(42));
    smart_ptr::WeakPtr<int> weak(shared);

    expect(!weak.expired(), "expected not expired");
    expect(weak.use_count() == 1, "expected use_count 1");

    auto locked = weak.lock();
    expect(*locked == 42, "expected 42");
}

void test_weak_ptr_expired() {
    smart_ptr::WeakPtr<int> weak;

    {
        smart_ptr::SharedPtr<int> shared(new int(10));
        weak = shared;
    }

    expect(weak.expired(), "expected expired");
}

void test_weak_ptr_lock() {
    smart_ptr::WeakPtr<int> weak;

    {
        smart_ptr::SharedPtr<int> shared(new int(20));
        weak = shared;

        auto locked = weak.lock();
        expect(*locked == 20, "expected 20");
    }

    auto locked = weak.lock();
    expect(locked == nullptr, "expected nullptr when expired");
}

void test_weak_ptr_prevent_cycle() {
    struct Node {
        smart_ptr::SharedPtr<Node> next;
        smart_ptr::WeakPtr<Node> prev;
        int value;
        Node(int v) : value(v) {}
    };

    auto node1 = smart_ptr::SharedPtr<Node>(new Node(1));
    auto node2 = smart_ptr::SharedPtr<Node>(new Node(2));

    node1->next = node2;
    node2->prev = node1;  // Weak reference, no cycle

    expect(node1.use_count() == 1, "expected use_count 1");
    expect(node2.use_count() == 2, "expected use_count 2");
}

} // namespace

int main() {
    try {
        std::cout << "Smart Pointer Tests:\n";

        std::cout << "\n  UniquePtr:\n";
        test_unique_ptr_basic();
        std::cout << "    test_unique_ptr_basic: PASS\n";

        test_unique_ptr_move();
        std::cout << "    test_unique_ptr_move: PASS\n";

        test_unique_ptr_release();
        std::cout << "    test_unique_ptr_release: PASS\n";

        test_unique_ptr_reset();
        std::cout << "    test_unique_ptr_reset: PASS\n";

        test_unique_ptr_bool();
        std::cout << "    test_unique_ptr_bool: PASS\n";

        std::cout << "\n  SharedPtr:\n";
        test_shared_ptr_basic();
        std::cout << "    test_shared_ptr_basic: PASS\n";

        test_shared_ptr_copy();
        std::cout << "    test_shared_ptr_copy: PASS\n";

        test_shared_ptr_move();
        std::cout << "    test_shared_ptr_move: PASS\n";

        test_shared_ptr_assignment();
        std::cout << "    test_shared_ptr_assignment: PASS\n";

        test_shared_ptr_reset();
        std::cout << "    test_shared_ptr_reset: PASS\n";

        test_shared_ptr_unique();
        std::cout << "    test_shared_ptr_unique: PASS\n";

        std::cout << "\n  WeakPtr:\n";
        test_weak_ptr_basic();
        std::cout << "    test_weak_ptr_basic: PASS\n";

        test_weak_ptr_expired();
        std::cout << "    test_weak_ptr_expired: PASS\n";

        test_weak_ptr_lock();
        std::cout << "    test_weak_ptr_lock: PASS\n";

        test_weak_ptr_prevent_cycle();
        std::cout << "    test_weak_ptr_prevent_cycle: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "smart_ptr_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nsmart_ptr_tests passed" << std::endl;
    return 0;
}
