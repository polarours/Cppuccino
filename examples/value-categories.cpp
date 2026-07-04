// examples/value-categories.cpp
// Demonstrates lvalue, rvalue, prvalue, xvalue, and glvalue.
// Compile with: g++ -std=c++17 -o value_categories value-categories.cpp

#include <iostream>
#include <string>
#include <utility>

// --- 1. Lvalues ---

void lvalues() {
    std::cout << "--- Lvalues ---\n";

    int x = 42;           // x is an lvalue
    int* p = &x;          // &x is valid because x is an lvalue
    std::string s = "hi"; // s is an lvalue

    std::cout << "x = " << x << "\n";
    std::cout << "&x = " << p << "\n";

    // Lvalue reference
    int& ref = x;         // OK: lvalue reference to lvalue
    ref = 100;
    std::cout << "After ref = 100: x = " << x << "\n";
}

// --- 2. Rvalues and Prvalues ---

void rvaluesAndPrvalues() {
    std::cout << "\n--- Rvalues and Prvalues ---\n";

    int x = 42;
    int y = x + 1;        // x + 1 is a prvalue (pure rvalue)
    std::string s1 = "hi";
    std::string s2 = s1 + "!";  // s1 + "!" is a prvalue

    std::cout << "x + 1 = " << y << "\n";
    std::cout << "s1 + \"!\" = " << s2 << "\n";

    // Temporary objects are prvalues
    int&& rref = 42;      // OK: rvalue reference to prvalue
    std::cout << "rref = " << rref << "\n";

    // Rvalue reference to function call
    int&& rref2 = x + 1;
    std::cout << "rref2 = " << rref2 << "\n";
}

// --- 3. Xvalues (Expiring Values) ---

void xvalues() {
    std::cout << "\n--- Xvalues ---\n";

    std::string s = "hello";
    std::string&& rref = std::move(s);  // std::move(s) is an xvalue

    std::cout << "rref = " << rref << "\n";
    std::cout << "s after move = \"" << s << "\" (valid but unspecified)\n";

    // Cast to rvalue reference creates xvalue
    int x = 42;
    int&& moved = static_cast<int&&>(x);
    std::cout << "moved = " << moved << "\n";
}

// --- 4. Function Parameter Categories ---

void byValue(int x) {
    std::cout << "byValue: " << x << "\n";
}

void byLvalueRef(int& x) {
    std::cout << "byLvalueRef: " << x << "\n";
}

void byRvalueRef(int&& x) {
    std::cout << "byRvalueRef: " << x << "\n";
}

void byConstLvalueRef(const int& x) {
    std::cout << "byConstLvalueRef: " << x << "\n";
}

void functionParameterCategories() {
    std::cout << "\n--- Function Parameter Categories ---\n";

    int x = 42;

    byValue(x);           // lvalue
    byValue(42);          // prvalue

    byLvalueRef(x);       // lvalue
    // byLvalueRef(42);   // ERROR: cannot bind lvalue ref to rvalue

    byRvalueRef(42);      // prvalue
    byRvalueRef(std::move(x));  // xvalue

    byConstLvalueRef(x);  // lvalue
    byConstLvalueRef(42); // prvalue (OK: const lvalue ref can bind to rvalue)
}

// --- 5. Move Semantics and Value Categories ---

class Buffer {
public:
    Buffer(size_t size) : size_(size), data_(new int[size]) {
        std::cout << "  Constructor: size=" << size_ << "\n";
    }

    ~Buffer() {
        delete[] data_;
        std::cout << "  Destructor: size=" << size_ << "\n";
    }

    // Move constructor takes rvalue reference
    Buffer(Buffer&& other) noexcept
        : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;
        std::cout << "  Move constructor\n";
    }

    // Copy constructor takes lvalue reference
    Buffer(const Buffer& other)
        : size_(other.size_), data_(new int[other.size_]) {
        std::copy(other.data_, other.data_ + other.size_, data_);
        std::cout << "  Copy constructor: size=" << size_ << "\n";
    }

    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;
};

void moveSemanticsAndValueCategories() {
    std::cout << "\n--- Move Semantics and Value Categories ---\n";

    Buffer b1(10);                    // lvalue: calls constructor
    Buffer b2(b1);                    // lvalue: calls copy constructor
    Buffer b3(std::move(b1));         // xvalue: calls move constructor
    Buffer b4(Buffer(20));            // prvalue: may use copy elision
}

// --- 6. Expression Categories ---

void expressionCategories() {
    std::cout << "\n--- Expression Categories ---\n";

    int x = 42;

    // Lvalue expressions
    std::cout << "x is lvalue\n";
    std::cout << "*(&x) is lvalue\n";

    // Prvalue expressions
    std::cout << "42 is prvalue\n";
    std::cout << "x + 1 is prvalue\n";
    std::cout << "std::string(\"hi\") is prvalue\n";

    // Xvalue expressions
    std::cout << "std::move(x) is xvalue\n";
    std::cout << "Buffer(10).size() is xvalue\n";
}

// --- 7. Universal References ---

template <typename T>
void universalRef(T&& arg) {
    // T&& is a universal/forwarding reference
    // It can bind to both lvalues and rvalues
    std::cout << "universalRef: " << arg << "\n";
}

void universalReferences() {
    std::cout << "\n--- Universal References ---\n";

    int x = 42;
    universalRef(x);        // T = int&, binds to lvalue
    universalRef(42);       // T = int, binds to rvalue
    universalRef(std::move(x));  // T = int, binds to xvalue
}

// --- 8. const and Value Categories ---

void constAndValueCategories() {
    std::cout << "\n--- const and Value Categories ---\n";

    const int x = 42;

    // const lvalue reference can bind to anything
    const int& ref1 = x;      // lvalue
    const int& ref2 = 42;     // prvalue
    const int& ref3 = std::move(x);  // xvalue

    std::cout << "ref1 = " << ref1 << "\n";
    std::cout << "ref2 = " << ref2 << "\n";
    std::cout << "ref3 = " << ref3 << "\n";
}

int main() {
    lvalues();
    rvaluesAndPrvalues();
    xvalues();
    functionParameterCategories();
    moveSemanticsAndValueCategories();
    expressionCategories();
    universalReferences();
    constAndValueCategories();

    std::cout << "\n=== All value category scenarios completed ===\n";
    return 0;
}
