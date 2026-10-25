// examples/pitfall-const-object-move-trap.cpp
// Demonstrates that std::move applied to a const object silently copies instead of moving.
// Compile with: g++ -std=c++17 -o pitfall_const_object_move_trap pitfall-const-object-move-trap.cpp

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

// A type that reports whether it was copied or moved.
class Buffer {
public:
    explicit Buffer(std::string data) : data_(std::move(data)) {}

    Buffer(const Buffer& other) : data_(other.data_) {
        std::cout << "      [copy constructor ran]\n";
    }

    Buffer(Buffer&& other) noexcept : data_(std::move(other.data_)) {
        std::cout << "      [move constructor ran]\n";
    }

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    const std::string& data() const { return data_; }

private:
    std::string data_;
};

// --- 1. The trap: a move from a const object is a copy ---

void silentCopyInsteadOfMove() {
    std::cout << "--- 1. std::move on a const object copies ---\n";

    const Buffer source{"payload"};

    // std::move is only a cast. On a const object the result is a
    // `const Buffer&&`, which the move constructor cannot bind.
    static_assert(std::is_same_v<decltype(std::move(source)), const Buffer&&>,
                  "std::move on a const object yields a const rvalue reference");

    // The trap: this compiles, but it copies. Overload resolution falls back
    // to the copy constructor, so every byte is duplicated.
    Buffer result = std::move(source);

    std::cout << "    result holds: \"" << result.data() << "\"\n";
    std::cout << "    source still holds: \"" << source.data()
              << "\" (nothing was taken away)\n";
}

// --- 2. A move-only type does not even compile ---

void moveOnlyTypeDoesNotCompile() {
    std::cout << "\n--- 2. A move-only type refuses to compile ---\n";

    // BAD: this is a compile error, not a runtime failure.
    // const std::unique_ptr<int> p = std::make_unique<int>(42);
    // auto q = std::move(p);
    //   error: use of deleted function
    //          'std::unique_ptr<int>::unique_ptr(const unique_ptr&)'
    //
    // The copy constructor of unique_ptr is deleted, so the silent fallback
    // that section 1 relies on is not available here.

    std::unique_ptr<int> owned = std::make_unique<int>(42);
    auto moved = std::move(owned);  // OK: the source is not const
    std::cout << "    a non-const source moves fine: *moved == " << *moved << "\n";
}

// --- 3. The correct way to move ---

void correctMove() {
    std::cout << "\n--- 3. Moving from a non-const source ---\n";

    Buffer source{"payload"};
    Buffer moved = std::move(source);  // the move constructor runs

    std::cout << "    moved holds: \"" << moved.data() << "\"\n";
    std::cout << "    source after the move: \"" << source.data()
              << "\" (its state is unspecified)\n";
}

// --- 4. Read-only access and explicit copies ---

void readOnlyAccessAndExplicitCopy() {
    std::cout << "\n--- 4. Read-only access and explicit copies ---\n";

    const Buffer source{"payload"};

    const Buffer& view = source;  // read-only access, no copy at all
    std::cout << "    view holds: \"" << view.data() << "\"\n";

    Buffer copy = source;  // an explicit copy, and it says so in the code
    std::cout << "    explicit copy holds: \"" << copy.data() << "\"\n";
}

// --- 5. A const-safe interface ---

class Data {
public:
    Data() = default;
    explicit Data(int value) : data_(std::make_unique<int>(value)) {}

    // The copy constructor is the const-safe way to take data out of a
    // const object: it makes an independent copy.
    Data(const Data& other)
        : data_(other.data_ ? std::make_unique<int>(*other.data_) : nullptr) {}

    // The move constructor takes ownership away from a non-const source.
    Data(Data&& other) noexcept = default;

    Data& operator=(const Data&) = delete;
    Data& operator=(Data&&) = delete;

    int value() const { return data_ ? *data_ : 0; }

private:
    std::unique_ptr<int> data_;
};

void constSafeInterface() {
    std::cout << "\n--- 5. A const-safe interface ---\n";

    const Data original{42};

    // Deep copy through the copy constructor: defined behaviour, no surprise.
    Data copy = original;
    std::cout << "    copy holds: " << copy.value() << "\n";
}

int main() {
    silentCopyInsteadOfMove();
    moveOnlyTypeDoesNotCompile();
    correctMove();
    readOnlyAccessAndExplicitCopy();
    constSafeInterface();

    std::cout << "\n=== All const-object move scenarios completed ===\n";
    return 0;
}
