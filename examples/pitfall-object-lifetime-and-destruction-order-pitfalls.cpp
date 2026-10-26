// examples/pitfall-object-lifetime-and-destruction-order-pitfalls.cpp
// Demonstrates object lifetime rules: destruction order, dangling access and shared_ptr cycles.
// Compile with: g++ -std=c++17 -o pitfall_object_lifetime_and_destruction_order_pitfalls pitfall-object-lifetime-and-destruction-order-pitfalls.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// A helper that announces its own construction and destruction.
struct Tracked {
    explicit Tracked(std::string label) : name(std::move(label)) {
        std::cout << "  constructed " << name << "\n";
    }
    ~Tracked() { std::cout << "  destroyed  " << name << "\n"; }

    Tracked(const Tracked&) = default;
    Tracked& operator=(const Tracked&) = default;
    Tracked(Tracked&&) = default;
    Tracked& operator=(Tracked&&) = default;

    std::string name;
};

// --- 1. Stack objects: destroyed in reverse order of construction ---

void stackDestructionOrder() {
    std::cout << "--- 1. Stack objects: reverse order of construction ---\n";
    {
        Tracked first("first");
        Tracked second("second");
        Tracked third("third");
        std::cout << "  (leaving the scope)\n";
    }
}

// --- 2. Class members: destroyed in reverse order of declaration ---

class Widget {
public:
    Widget() { std::cout << "  Widget constructed\n"; }
    ~Widget() { std::cout << "  Widget destroyed\n"; }

private:
    Tracked name_{"member: name"};  // declared first  -> destroyed second
    Tracked data_{"member: data"};  // declared second -> destroyed first
};

void memberDestructionOrder() {
    std::cout << "\n--- 2. Class members: reverse order of declaration ---\n";
    Widget widget;
}

// --- 3. Base classes: destroyed after the derived class ---

class Base {
public:
    Base() { std::cout << "  Base constructed\n"; }
    virtual ~Base() { std::cout << "  Base destroyed\n"; }  // virtual: polymorphic base
};

class Derived : public Base {
public:
    Derived() { std::cout << "  Derived constructed\n"; }
    ~Derived() override { std::cout << "  Derived destroyed\n"; }
};

void baseDerivedDestructionOrder() {
    std::cout << "\n--- 3. Base classes are destroyed last ---\n";
    Derived derived;
}

// --- 4. A reference to a local that dies ---

// BAD: returns a reference to a local, which is destroyed on the way out.
// std::string& badGreeting() {
//     std::string greeting = "Hello";
//     return greeting;
// }

std::string goodGreeting() {
    return "Hello";  // OK: returned by value
}

void danglingReferenceToLocal() {
    std::cout << "\n--- 4. A reference to a local that dies ---\n";

    // BAD: `greeting` is destroyed when badGreeting() returns, so the
    // reference dangles -- reading it is undefined behaviour.
    // std::string& greeting = badGreeting();
    // std::cout << "  " << greeting << "\n";

    // GOOD: return by value, the caller owns the result.
    std::string greeting = goodGreeting();
    std::cout << "  " << greeting << "\n";
}

// --- 5. Iterator invalidation ---

void iteratorInvalidation() {
    std::cout << "\n--- 5. Iterator invalidation on erase ---\n";

    std::vector<int> values = {1, 2, 3, 4, 5};

    // BAD: `erase` invalidates the iterator it was given, so the `++it`
    // walks past the end of the container -- undefined behaviour.
    // for (auto it = values.begin(); it != values.end(); ++it) {
    //     if (*it == 3) { values.erase(it); }
    // }

    // GOOD: continue from the iterator that `erase` returns.
    for (auto it = values.begin(); it != values.end();) {
        if (*it == 3) {
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    for (int value : values) {
        std::cout << "  value " << value << "\n";
    }
}

// --- 6. shared_ptr cycles ---

struct Node {
    std::shared_ptr<Node> next;  // strong reference
    std::weak_ptr<Node> prev;    // weak reference: does not keep the peer alive
};

void sharedPointerCycle() {
    std::cout << "\n--- 6. shared_ptr cycles ---\n";

    auto a = std::make_shared<Node>();
    auto b = std::make_shared<Node>();
    a->next = b;
    b->next = a;  // strong <-> strong: a cycle that nothing can collect
    std::cout << "  a.use_count() = " << a.use_count() << " (the cycle keeps both alive)\n";

    // BAD: leaving the scope without breaking the cycle leaks both nodes;
    // the sanitizers would report "ERROR: LeakSanitizer: detected memory leaks".
    // Break the cycle before the scope ends:
    a->next.reset();
    b->next.reset();
    std::cout << "  after breaking the cycle, a.use_count() = " << a.use_count() << "\n";

    // GOOD: express the back link with std::weak_ptr so no cycle can form.
    auto c = std::make_shared<Node>();
    auto d = std::make_shared<Node>();
    c->next = d;
    d->prev = c;
    std::cout << "  with a weak_ptr back link, c.use_count() = " << c.use_count() << "\n";
}

// --- 7. Static storage duration ---

int& configValue() {
    // A function-local static is constructed on first use and destroyed at
    // program exit: the "construct on first use" idiom that avoids the
    // static initialisation order fiasco between translation units.
    static int value = 42;
    return value;
}

void staticStorageDuration() {
    std::cout << "\n--- 7. Static storage duration ---\n";
    std::cout << "  configValue() = " << configValue() << "\n";
    std::cout << "  configValue() = " << configValue() << " (constructed only once)\n";

    // BAD: across translation units the order in which static objects are
    // constructed and destroyed is undefined, so a static object must not
    // depend on another translation unit's static object. A
    // `thread_local` object has the same problem relative to its thread:
    // it is destroyed when the thread exits.
}

int main() {
    stackDestructionOrder();
    memberDestructionOrder();
    baseDerivedDestructionOrder();
    danglingReferenceToLocal();
    iteratorInvalidation();
    sharedPointerCycle();
    staticStorageDuration();

    std::cout << "\n=== All object lifetime scenarios completed ===\n";
    return 0;
}
