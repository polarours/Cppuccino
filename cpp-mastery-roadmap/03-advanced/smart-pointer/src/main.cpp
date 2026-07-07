#include "smart_ptr.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== Smart Pointer Demo ===\n\n";

    // UniquePtr demo
    std::cout << "UniquePtr:\n";
    {
        smart_ptr::UniquePtr<std::string> ptr(new std::string("Hello"));
        std::cout << "  Value: " << *ptr << "\n";
        std::cout << "  Use count: 1 (unique ownership)\n";

        smart_ptr::UniquePtr<std::string> moved = std::move(ptr);
        std::cout << "  After move: " << *moved << "\n";
        std::cout << "  Original is " << (ptr ? "valid" : "null") << "\n";
    }
    std::cout << "  Resource automatically freed\n\n";

    // SharedPtr demo
    std::cout << "SharedPtr:\n";
    {
        smart_ptr::SharedPtr<int> ptr1(new int(42));
        std::cout << "  ptr1: " << *ptr1 << ", use_count: " << ptr1.use_count() << "\n";

        {
            smart_ptr::SharedPtr<int> ptr2 = ptr1;
            smart_ptr::SharedPtr<int> ptr3 = ptr1;
            std::cout << "  After 2 copies: use_count = " << ptr1.use_count() << "\n";
        }
        std::cout << "  After scope: use_count = " << ptr1.use_count() << "\n";
    }
    std::cout << "  Resource freed when last owner released\n\n";

    // WeakPtr demo
    std::cout << "WeakPtr:\n";
    {
        smart_ptr::SharedPtr<int> shared(new int(100));
        smart_ptr::WeakPtr<int> weak = shared;

        std::cout << "  weak.expired(): " << std::boolalpha << weak.expired() << "\n";

        if (auto locked = weak.lock()) {
            std::cout << "  Locked value: " << *locked << "\n";
        }
    }
    std::cout << "  After shared destroyed: weak.expired() = " << std::boolalpha
              << smart_ptr::WeakPtr<int>().expired() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
