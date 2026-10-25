// examples/pitfall-double-free.cpp
// Demonstrates double-free bugs and the ownership patterns that prevent them.
// Compile with: g++ -std=c++17 -o pitfall_double_free pitfall-double-free.cpp

#include <iostream>
#include <memory>
#include <utility>

// --- 1. Two names for the same allocation ---

void aliasingDoubleFree() {
    std::cout << "--- 1. Aliasing a raw pointer ---\n";

    // BAD: p and q are two names for one allocation, so the second delete
    // hands the same block back to the allocator twice (undefined behaviour).
    // int* p = new int(42);
    // int* q = p;        // q aliases p, it does NOT own a separate object
    // delete p;          // the int is freed here
    // delete q;          // DOUBLE FREE: q points at the already freed block
    //                   // AddressSanitizer would report:
    //                   //   attempting double-free on 0x... in thread T0

    // GOOD: exactly one owner frees the block exactly once.
    {
        auto owned = std::make_unique<int>(42);
        std::cout << "  unique_ptr owns the value " << *owned << "\n";
    }
    std::cout << "  the block was freed exactly once when the unique_ptr died\n";
}

// --- 2. A callee that "helpfully" deletes the caller's pointer ---

void consumeRawPointer(int* data) {
    // BAD version of this function:
    //   delete data;    // consumes ownership, but the signature never said so
    std::cout << "  consumeRawPointer() borrowed the value " << *data << "\n";
}

void consumeUniquePointer(std::unique_ptr<int> data) {
    *data += 1;  // unique_ptr owns the int, we may read and modify it
    std::cout << "  consumeUniquePointer() used the value " << *data << "\n";
    // `data` is destroyed here: freed exactly once, no matter what happens next
}

void consumeAndDelete() {
    std::cout << "\n--- 2. Ownership across a function boundary ---\n";

    // BAD: the callee deletes the block and the caller deletes it again.
    // int* raw = new int(7);
    // consumeRawPointer(raw);
    // delete raw;        // DOUBLE FREE: raw was already freed inside the callee

    // GOOD: the callee only borrows, the caller keeps the ownership.
    {
        auto borrowed = std::make_unique<int>(7);
        consumeRawPointer(borrowed.get());  // borrowed.get() is an observer, not an owner
    }

    // GOOD: ownership is moved into the callee, so only one site can free it.
    auto owned = std::make_unique<int>(7);
    consumeUniquePointer(std::move(owned));  // owned is empty afterwards
    std::cout << "  after the move, the caller owns nothing: " << (owned == nullptr) << "\n";
}

// --- 3. shared_ptr is not a licence to free twice ---

void sharedPointerCaveat() {
    std::cout << "\n--- 3. shared_ptr: copy the handle, never re-wrap the raw pointer ---\n";

    // BAD: each shared_ptr built from the same raw pointer creates its own
    // control block, so both think they are the sole owner.
    // int* raw = new int(5);
    // std::shared_ptr<int> a(raw);
    // std::shared_ptr<int> b(raw);  // second owner of an already owned block
    // a.reset();                     // frees the block
    // b.reset();                     // DOUBLE FREE

    // GOOD: one control block, two handles that share it.
    auto owner = std::make_shared<int>(5);
    auto observer = owner;  // copies the handle, not the ownership
    std::cout << "  use_count with two shared handles: " << owner.use_count() << "\n";
    owner.reset();
    std::cout << "  after owner.reset(): use_count " << observer.use_count()
              << ", value still alive: " << *observer << "\n";
}

// --- 4. A class that owns a raw pointer ---

class LegacyBuffer {
public:
    explicit LegacyBuffer(int value) : data_(new int(value)) {}

    ~LegacyBuffer() {
        delete data_;
        data_ = nullptr;
    }

    // A silently copied raw pointer would be a double free waiting to happen,
    // so the copy operations are removed instead of hidden.
    LegacyBuffer(const LegacyBuffer&) = delete;
    LegacyBuffer& operator=(const LegacyBuffer&) = delete;

    LegacyBuffer(LegacyBuffer&& other) noexcept : data_(other.data_) { other.data_ = nullptr; }

    LegacyBuffer& operator=(LegacyBuffer&& other) noexcept {
        if (this != &other) {
            delete data_;
            data_ = other.data_;
            other.data_ = nullptr;
        }
        return *this;
    }

    int* get() const { return data_; }

private:
    int* data_ = nullptr;
};

void owningClass() {
    std::cout << "\n--- 4. A class that owns its resource ---\n";

    LegacyBuffer buffer(99);
    std::cout << "  buffer owns the value " << *buffer.get() << "\n";

    // BAD: copying would give two owners of one raw pointer.
    // LegacyBuffer copy = buffer;         // compile error: copy constructor deleted
    // LegacyBuffer assigned(1);
    // assigned = buffer;                   // compile error: copy assignment deleted

    // Moving transfers the single owner.
    LegacyBuffer moved(std::move(buffer));
    std::cout << "  after the move, the moved-to buffer owns the value " << *moved.get() << "\n";
    std::cout << "  the moved-from buffer owns nothing: " << (buffer.get() == nullptr) << "\n";
}

int main() {
    aliasingDoubleFree();
    consumeAndDelete();
    sharedPointerCaveat();
    owningClass();

    std::cout << "\n=== All double free scenarios completed ===\n";
    return 0;
}
