// examples/pitfall-dangling-pointer.cpp
// Demonstrates dangling-pointer scenarios (use-after-free) and the ownership patterns that prevent them.
// Compile with: g++ -std=c++17 -o pitfall_dangling_pointer pitfall-dangling-pointer.cpp

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// --- 1. Raw pointer left dangling after delete ---

int* makeRawInt() {
    int* p = new int(42);
    return p;
}

void danglingAfterDelete() {
    std::cout << "--- 1. Raw pointer dangling after delete ---\n";

    // BAD: `raw` still holds the address of the block that was just freed.
    // Dereferencing it is undefined behaviour (use-after-free): the program
    // may print garbage, crash, or -- worst of all -- look perfectly fine.
    // int* raw = makeRawInt();
    // delete raw;
    // std::cout << *raw << "\n";   // UB: reads freed memory

    // GOOD: null the pointer right after the delete, so a stray access is
    // caught by a comparison instead of by a crash.
    int* raw = makeRawInt();
    std::cout << "  value before delete: " << *raw << "\n";
    delete raw;
    raw = nullptr;
    std::cout << "  pointer nulled after delete: " << (raw == nullptr ? "yes" : "no") << "\n";
}

// --- 2. Pointer into a freed array block ---

int* elementOfFreedArray() {
    int* arr = new int[3]{1, 2, 3};
    int* element = &arr[1];   // points into the middle of the block
    delete[] arr;             // the whole block is released
    return element;           // dangling: points inside freed memory
}

void danglingIntoFreedArray() {
    std::cout << "\n--- 2. Pointer into a freed array ---\n";

    // BAD: `element` points inside a block that no longer exists.
    // int* element = elementOfFreedArray();
    // std::cout << *element << "\n";   // UB: use-after-free

    // GOOD: keep the owner alive as long as anyone reads through the pointer.
    auto arr = std::make_unique<int[]>(3);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    int* element = &arr[1];
    std::cout << "  element = " << *element << " (the owner is still alive)\n";
}

// --- 3. Pointer invalidated by container growth ---

void invalidatedByContainerGrowth() {
    std::cout << "\n--- 3. Pointer invalidated by container growth ---\n";

    std::vector<int> values;
    values.reserve(1);
    values.push_back(1);

    // The address is copied into an integer *while the pointer is still
    // valid*; comparing integers afterwards is well defined.
    const std::uintptr_t oldAddress = reinterpret_cast<std::uintptr_t>(values.data());
    const std::size_t capacityBefore = values.capacity();

    // BAD: push_back reallocates the buffer and releases the old one, so
    // every pointer into the previous buffer dangles.
    // int* first = &values[0];
    // values.push_back(2);
    // std::cout << *first << "\n";   // UB if the buffer was reallocated

    values.push_back(2);
    const std::uintptr_t newAddress = reinterpret_cast<std::uintptr_t>(values.data());
    std::cout << "  capacity before/after growth: " << capacityBefore << "/" << values.capacity() << "\n";
    std::cout << "  buffer moved: " << (oldAddress != newAddress ? "yes" : "no")
              << " -- the old buffer was released\n";

    // GOOD: index into the container every time; indices stay valid.
    for (std::size_t i = 0; i < values.size(); ++i) {
        std::cout << "  values[" << i << "] = " << values[i] << "\n";
    }
}

// --- 4. Lambda that captures a local by reference ---

std::function<void()> makeBadCallback() {
    int tmp = 42;
    // BAD: `tmp` is captured by reference, but it dies when this function
    // returns -- the callback is left holding a dangling reference.
    return [&]() { std::cout << "  The value is: " << tmp << "\n"; };
}

std::function<void()> makeGoodCallback() {
    // GOOD: capture by value, so the lambda owns everything it needs.
    int tmp = 42;
    return [tmp]() { std::cout << "  The value is: " << tmp << "\n"; };
}

void danglingLambdaCapture() {
    std::cout << "\n--- 4. Lambda capturing a dead local ---\n";

    // BAD: invoking the callback reads a destroyed object -- undefined
    // behaviour, so it is not run here.
    // auto bad = makeBadCallback();
    // bad();

    // GOOD: the lambda owns its captured state.
    auto good = makeGoodCallback();
    good();
}

// --- 5. The fix: let an owner express the lifetime ---

class Session {
public:
    explicit Session(std::string user) : user_(std::move(user)) {}
    const std::string& user() const { return user_; }

private:
    std::string user_;
};

std::shared_ptr<Session> openSession() {
    // GOOD: the shared_ptr owns the session, so every holder keeps it alive.
    return std::make_shared<Session>("alice");
}

void ownershipInsteadOfAddresses() {
    std::cout << "\n--- 5. Let an owner express the lifetime ---\n";

    auto session = openSession();
    std::cout << "  session user: " << session->user() << "\n";
    std::cout << "  use_count while `session` is alive: " << session.use_count() << "\n";

    std::weak_ptr<Session> observer = session;
    session.reset();
    std::cout << "  after reset, the observer sees an expired object: "
              << (observer.expired() ? "yes" : "no") << "\n";
}

int main() {
    danglingAfterDelete();
    danglingIntoFreedArray();
    invalidatedByContainerGrowth();
    danglingLambdaCapture();
    ownershipInsteadOfAddresses();

    std::cout << "\n=== All dangling pointer scenarios completed ===\n";
    return 0;
}
