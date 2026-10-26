// examples/pitfall-const-correctness-pitfalls.cpp
// Demonstrates const correctness pitfalls: temporary lifetime, mutable state, and casting away const.
// Compile with: g++ -std=c++17 -o pitfall_const_correctness_pitfalls pitfall-const-correctness-pitfalls.cpp

#include <iostream>
#include <map>
#include <string>
#include <string_view>

// --- 1. Lifetime of a temporary: reference vs pointer ---

void temporaryLifetime() {
    std::cout << "--- Temporary lifetime ---\n";

    // GOOD: a const reference extends the lifetime of the temporary it binds to.
    const std::string& ref = std::string("temporary");
    std::cout << "const ref to temporary: " << ref << "\n";

    // BAD: c_str() returns a pointer, and a pointer does NOT extend the
    // lifetime of the temporary. The std::string is destroyed at the end of
    // the full expression, so the pointer dangles.
    // const char* ptr = std::string("temporary").c_str();
    // std::cout << ptr << "\n";   // undefined behavior: reads freed memory

    // GOOD: keep the string alive for as long as the pointer is used.
    std::string owner = "temporary";
    const char* ptr = owner.c_str();
    std::cout << "pointer into a live string: " << ptr << "\n";
}

// --- 2. const member functions and mutable state ---

class Cache {
public:
    int get(int key) const {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;  // OK: reading from a const member
        }
        int value = key * key;
        // Without `mutable` this line is a compile error: a const member
        // function cannot call the non-const operator[] of the map.
        cache_[key] = value;
        return value;
    }

    std::size_t size() const { return cache_.size(); }

private:
    mutable std::map<int, int> cache_;  // mutable allows a const method to cache
};

void constMemberFunction() {
    std::cout << "\n--- const member function with a mutable cache ---\n";

    const Cache cache;
    std::cout << "get(3) = " << cache.get(3) << "\n";
    std::cout << "get(3) again = " << cache.get(3) << "\n";
    std::cout << "cached entries: " << cache.size() << "\n";
}

// --- 3. Casting away const ---

class Buffer {
public:
    explicit Buffer(int value) : value_(value) {}

    int value() const { return value_; }

    // Legal use of const_cast: hand out a non-const view of internal state.
    int* mutable_view() { return const_cast<int*>(&value_); }

private:
    int value_;
};

void castAwayConst() {
    std::cout << "\n--- Casting away const ---\n";

    // BAD: writing through a const_cast to a truly const object is undefined
    // behavior, so it is not executed here.
    // const int x = 42;
    // int* p = const_cast<int*>(&x);
    // *p = 100;   // undefined behavior

    // GOOD: the object was originally non-const, so removing const is legal.
    int value = 42;
    const int& const_ref = value;
    int* p = const_cast<int*>(&const_ref);
    *p = 100;
    std::cout << "non-const object written through const_cast: " << value << "\n";

    // GOOD: const_cast on a member of a non-const object is also fine.
    Buffer buffer(7);
    *buffer.mutable_view() = 9;
    std::cout << "buffer value: " << buffer.value() << "\n";
}

// --- 4. const parameters and const return values ---

std::string_view view_of(const std::string& text) {
    return text;  // OK: a const reference parameter does not dangle here
}

void constParameters() {
    std::cout << "\n--- const parameters ---\n";

    std::string text = "hello";
    std::cout << "view: " << view_of(text) << "\n";
    text += " world";
    std::cout << "view after the owner grows: " << view_of(text) << "\n";
}

int main() {
    temporaryLifetime();
    constMemberFunction();
    castAwayConst();
    constParameters();

    std::cout << "\n=== All const correctness scenarios completed ===\n";
    return 0;
}
