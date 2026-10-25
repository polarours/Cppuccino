// examples/pitfall-dangling-lambda-captures.cpp
// Demonstrates dangling lambda captures and the ownership patterns that prevent them.
// Compile with: g++ -std=c++17 -o pitfall_dangling_lambda_captures pitfall-dangling-lambda-captures.cpp

#include <functional>
#include <iostream>
#include <memory>
#include <string>

using Callback = std::function<void()>;

// --- 1. Returning a lambda that captured a local by reference ---

std::function<int()> makeBadByReference() {
    int x = 7;
    return [&x]() { return x; };  // BAD: the closure only stores a reference to x
}

std::function<int()> makeGoodByValue() {
    int x = 7;
    return [x]() { return x; };  // GOOD: the closure owns a copy of x
}

// --- 2. std::reference_wrapper does not extend the lifetime ---

std::function<int()> makeBadWithRef() {
    int x = 5;
    return [r = std::ref(x)]() { return r.get(); };  // BAD: the wrapper still points at x
}

// --- 3. Init-capture with move transfers ownership into the closure ---

// The closure is move-only (it owns a unique_ptr), so it cannot be stored in a
// std::function; returning the closure itself keeps the ownership transfer intact.
auto makeGoodWithMoveCapture() {
    auto owned = std::make_unique<int>(99);
    return [p = std::move(owned)]() { return *p; };  // GOOD: the closure owns the pointee
}

// --- 4. Capturing `this` in a deferred callback ---

class Worker {
public:
    explicit Worker(int value) : value_(value) {}

    ~Worker() { std::cout << "  Worker destroyed\n"; }

    // BAD: the callback stores the address of this; it must not outlive the Worker.
    void scheduleUnsafe(Callback& slot) { slot = [this]() { useValue(); }; }

    // GOOD: capture a snapshot of the data instead of the object address.
    void scheduleSafe(Callback& slot) { slot = [v = value_]() { std::cout << "  callback used snapshot value " << v << "\n"; }; }

    int useValue() const { return value_; }

private:
    int value_;
};

// --- 5. Shared / weak ownership for member callbacks ---

class SharedWorker : public std::enable_shared_from_this<SharedWorker> {
public:
    explicit SharedWorker(int value) : value_(value) {}

    void schedule(Callback& slot) {
        auto self = shared_from_this();
        slot = [self]() { std::cout << "  callback ran on live object, value " << self->value_ << "\n"; };
    }

    int value_ = 0;
};

// --- Demo sections ---

void returningLambda() {
    std::cout << "--- 1. Returning a lambda that escapes the scope ---\n";

    auto bad = makeBadByReference();
    auto good = makeGoodByValue();

    // BAD: x is destroyed when makeBadByReference() returns, so bad() reads a
    // dangling reference (undefined behaviour: garbage value or crash).
    // std::cout << "  bad() = " << bad() << "\n";

    // GOOD: the closure owns its own copy of x.
    std::cout << "  good() = " << good() << "\n";
    (void)bad;  // kept alive only to show that the closure itself is valid
}

void referenceWrapperIsNotAFix() {
    std::cout << "\n--- 2. std::reference_wrapper keeps no lifetime ---\n";

    auto bad = makeBadWithRef();

    // BAD: x is gone once makeBadWithRef() returns, so the wrapper dangles.
    // std::cout << "  bad() = " << bad() << "\n";

    std::cout << "  std::ref stores a reference, not an owner\n";
    (void)bad;
}

void moveCapture() {
    std::cout << "\n--- 3. Init-capture with std::move ---\n";

    auto good = makeGoodWithMoveCapture();
    std::cout << "  good() = " << good() << "\n";
}

void capturingThis() {
    std::cout << "\n--- 4. Capturing `this` in a deferred callback ---\n";

    Callback slot;
    {
        Worker worker(42);
        worker.scheduleUnsafe(slot);
        std::cout << "  scheduled an unsafe callback while the Worker is alive\n";
    }
    // The Worker is destroyed here, so calling slot() would use a dangling `this`.
    // slot();

    Callback safeSlot;
    {
        Worker worker(42);
        worker.scheduleSafe(safeSlot);
    }
    safeSlot();
}

void sharedOwnership() {
    std::cout << "\n--- 5. Explicit shared ownership keeps the callback valid ---\n";

    Callback slot;
    auto worker = std::make_shared<SharedWorker>(7);
    worker->schedule(slot);
    worker.reset();  // the closure still holds a reference, so the object stays alive
    slot();
}

int main() {
    returningLambda();
    referenceWrapperIsNotAFix();
    moveCapture();
    capturingThis();
    sharedOwnership();

    std::cout << "\n=== All dangling lambda capture scenarios completed ===\n";
    return 0;
}
