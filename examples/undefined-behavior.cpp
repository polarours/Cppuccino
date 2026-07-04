// examples/undefined-behavior.cpp
// Demonstrates common undefined behavior scenarios and how to avoid them.
// Compile with: g++ -std=c++17 -o undefined_behavior undefined-behavior.cpp

#include <iostream>
#include <vector>
#include <cstring>
#include <climits>
#include <memory>
#include <atomic>

// --- 1. Signed Integer Overflow ---

void signedIntegerOverflow() {
    std::cout << "--- Signed Integer Overflow ---\n";

    int x = INT_MAX;
    std::cout << "INT_MAX = " << x << "\n";

    // This is undefined behavior!
    // int y = x + 1;

    // Safe alternatives
    if (x < INT_MAX) {
        int y = x + 1;
        std::cout << "x + 1 = " << y << "\n";
    } else {
        std::cout << "Cannot add 1 without overflow\n";
    }

    // Use checked arithmetic
    int result;
    if (__builtin_add_overflow(x, 1, &result)) {
        std::cout << "Overflow detected!\n";
    }
}

// --- 2. Null Pointer Dereference ---

void nullPointerDereference() {
    std::cout << "\n--- Null Pointer Dereference ---\n";

    int* ptr = nullptr;

    // This is undefined behavior!
    // *ptr = 42;

    // Safe alternatives
    if (ptr != nullptr) {
        *ptr = 42;
    } else {
        std::cout << "ptr is null, cannot dereference\n";
    }

    // Use optional or check before use
    int value = ptr ? *ptr : 0;
    std::cout << "value = " << value << "\n";
}

// --- 3. Buffer Overflow ---

void bufferOverflow() {
    std::cout << "\n--- Buffer Overflow ---\n";

    int arr[5] = {1, 2, 3, 4, 5};

    // This is undefined behavior!
    // arr[10] = 42;

    // Safe alternatives
    size_t index = 2;
    if (index < sizeof(arr) / sizeof(arr[0])) {
        std::cout << "arr[" << index << "] = " << arr[index] << "\n";
    }

    // Use std::vector for bounds-checked access
    std::vector<int> vec = {1, 2, 3, 4, 5};
    try {
        std::cout << "vec.at(2) = " << vec.at(2) << "\n";
        std::cout << "vec.at(10) = " << vec.at(10) << "\n";
    } catch (const std::out_of_range& e) {
        std::cout << "Out of range: " << e.what() << "\n";
    }
}

// --- 4. Use After Free ---

void useAfterFree() {
    std::cout << "\n--- Use After Free ---\n";

    int* ptr = new int(42);
    std::cout << "Before delete: " << *ptr << "\n";

    delete ptr;

    // This is undefined behavior!
    // std::cout << "After delete: " << *ptr << "\n";

    // Safe alternative: set to nullptr
    ptr = nullptr;
    if (ptr) {
        std::cout << "ptr is valid\n";
    } else {
        std::cout << "ptr is null after delete\n";
    }

    // Better: use smart pointers
    auto smartPtr = std::make_unique<int>(42);
    std::cout << "Smart pointer: " << *smartPtr << "\n";
    smartPtr.reset();
}

// --- 5. Double Free ---

void doubleFree() {
    std::cout << "\n--- Double Free ---\n";

    int* ptr = new int(42);
    delete ptr;

    // This is undefined behavior!
    // delete ptr;

    // Safe alternative: set to nullptr after delete
    ptr = nullptr;

    // Or use smart pointers
    auto smartPtr = std::make_unique<int>(42);
    smartPtr.reset();
    // smartPtr.reset();  // Safe, no double free
}

// --- 6. Uninitialized Variables ---

void uninitializedVariables() {
    std::cout << "\n--- Uninitialized Variables ---\n";

    // This is undefined behavior (may print garbage)
    // int x;
    // std::cout << x << "\n";

    // Safe alternatives
    int x = 0;
    std::cout << "Initialized x = " << x << "\n";

    int y{};
    std::cout << "Value-initialized y = " << y << "\n";
}

// --- 7. Integer Division by Zero ---

void integerDivisionByZero() {
    std::cout << "\n--- Integer Division by Zero ---\n";

    int a = 10;
    int b = 0;

    // This is undefined behavior!
    // int result = a / b;

    // Safe alternative: check before dividing
    if (b != 0) {
        int result = a / b;
        std::cout << "result = " << result << "\n";
    } else {
        std::cout << "Cannot divide by zero\n";
    }

    // Floating point division by zero is defined (inf or nan)
    double da = 10.0;
    double db = 0.0;
    double result = da / db;
    std::cout << "10.0 / 0.0 = " << result << "\n";
}

// --- 8. Shift by Negative or Too Large ---

void shiftByNegative() {
    std::cout << "\n--- Shift by Negative or Too Large ---\n";

    int x = 1;
    int shift = 3;

    // This is undefined behavior!
    // int result = x << -1;
    // int result2 = x << 32;

    // Safe alternative: check shift amount
    if (shift >= 0 && shift < sizeof(int) * 8) {
        int result = x << shift;
        std::cout << "1 << " << shift << " = " << result << "\n";
    }
}

// --- 9. Out of Bounds Iterator ---

void outOfBoundsIterator() {
    std::cout << "\n--- Out of Bounds Iterator ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};

    // This is undefined behavior!
    // auto it = vec.begin() + 10;
    // std::cout << *it << "\n";

    // Safe alternatives
    size_t index = 2;
    if (index < vec.size()) {
        std::cout << "vec[" << index << "] = " << vec[index] << "\n";
    }

    // Use at() for bounds checking
    try {
        std::cout << "vec.at(10) = " << vec.at(10) << "\n";
    } catch (const std::out_of_range& e) {
        std::cout << "Out of range: " << e.what() << "\n";
    }
}

// --- 10. Strict Aliasing Violation ---

void strictAliasingViolation() {
    std::cout << "\n--- Strict Aliasing Violation ---\n";

    int x = 42;
    float* fp = reinterpret_cast<float*>(&x);

    // This is undefined behavior!
    // std::cout << "float value: " << *fp << "\n";

    // Safe alternative: use std::bit_cast (C++20) or memcpy
    float f;
    std::memcpy(&f, &x, sizeof(f));
    std::cout << "float via memcpy: " << f << "\n";
}

// --- 11. Return Reference to Local ---

int& badFunction() {
    int local = 42;
    return local;  // Undefined behavior!
}

int* goodFunction() {
    int* local = new int(42);
    return local;  // Caller must delete
}

void returnReferenceToLocal() {
    std::cout << "\n--- Return Reference to Local ---\n";

    // This is undefined behavior!
    // int& ref = badFunction();

    // Safe alternative: return by value or use heap
    int* ptr = goodFunction();
    std::cout << "value = " << *ptr << "\n";
    delete ptr;
}

// --- 12. Data Race ---

void dataRace() {
    std::cout << "\n--- Data Race ---\n";

    // This is undefined behavior!
    // int counter = 0;
    // std::thread t1([&]() { for(int i=0; i<1000; i++) counter++; });
    // std::thread t2([&]() { for(int i=0; i<1000; i++) counter++; });
    // t1.join(); t2.join();

    // Safe alternative: use atomic or mutex
    std::atomic<int> atomicCounter{0};
    // ... use atomic operations

    std::cout << "Use std::atomic or std::mutex for thread safety\n";
}

int main() {
    signedIntegerOverflow();
    nullPointerDereference();
    bufferOverflow();
    useAfterFree();
    doubleFree();
    uninitializedVariables();
    integerDivisionByZero();
    shiftByNegative();
    outOfBoundsIterator();
    strictAliasingViolation();
    returnReferenceToLocal();
    dataRace();

    std::cout << "\n=== All undefined behavior scenarios completed ===\n";
    return 0;
}
