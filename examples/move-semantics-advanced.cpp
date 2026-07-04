// examples/move-semantics-advanced.cpp
// Demonstrates advanced move semantics: NRVO, move vs copy, and common pitfalls.
// Compile with: g++ -std=c++17 -o move_advanced move-semantics-advanced.cpp

#include <iostream>
#include <string>
#include <vector>
#include <utility>

// --- 1. Named Return Value Optimization (NRVO) ---

class NrvObject {
public:
    NrvObject() { std::cout << "  [Default] constructed\n"; }
    NrvObject(const NrvObject&) { std::cout << "  [Copy] constructed\n"; }
    NrvObject(NrvObject&&) noexcept { std::cout << "  [Move] constructed\n"; }
    ~NrvObject() { std::cout << "  destroyed\n"; }
};

// NRVO: compiler may elide the copy/move entirely
NrvObject createNrvObject() {
    NrvObject obj;
    return obj;  // Single object: NRVO typically applied
}

// No NRVO: multiple return paths
NrvObject createNrvObjectConditional(bool flag) {
    NrvObject a, b;
    return flag ? a : b;  // Cannot apply NRVO to different objects
}

void nrvoDemo() {
    std::cout << "--- NRVO (Named Return Value Optimization) ---\n";

    std::cout << "Single return path (NRVO likely):\n";
    auto obj1 = createNrvObject();

    std::cout << "\nMultiple return paths (NRVO not possible):\n";
    auto obj2 = createNrvObjectConditional(true);
}

// --- 2. Move vs Copy Decision ---

class ExpensiveResource {
public:
    explicit ExpensiveResource(std::string name) : name_(std::move(name)) {
        std::cout << "  [Acquire] " << name_ << "\n";
    }

    ~ExpensiveResource() {
        std::cout << "  [Release] " << name_ << "\n";
    }

    // Copy: expensive
    ExpensiveResource(const ExpensiveResource& other) : name_(other.name_ + "_copy") {
        std::cout << "  [Copy] " << name_ << " (expensive!)\n";
    }

    // Move: cheap
    ExpensiveResource(ExpensiveResource&& other) noexcept : name_(std::move(other.name_)) {
        other.name_ = "";
        std::cout << "  [Move] " << name_ << " (cheap)\n";
    }

    const std::string& name() const { return name_; }

private:
    std::string name_;
};

void moveVsCopyDemo() {
    std::cout << "\n--- Move vs Copy Decision ---\n";

    ExpensiveResource original("data");

    std::cout << "\nCopy (expensive):\n";
    ExpensiveResource copy = original;  // Copy constructor
    std::cout << "original: " << original.name() << "\n";
    std::cout << "copy: " << copy.name() << "\n";

    std::cout << "\nMove (cheap):\n";
    ExpensiveResource moved = std::move(original);  // Move constructor
    std::cout << "original (moved-from): " << original.name() << "\n";
    std::cout << "moved: " << moved.name() << "\n";
}

// --- 3. When NOT to Move ---

void whenNotToMove() {
    std::cout << "\n--- When NOT to Move ---\n";

    // Don't move const objects (move becomes copy)
    const std::string constStr = "constant";
    std::string s1 = std::move(constStr);  // Copies! (move ctor takes const&)
    std::cout << "Moving const: still copies\n";

    // Don't move fundamental types (no benefit)
    int x = 42;
    int y = std::move(x);  // Same as copy
    std::cout << "Moving int: same as copy\n";

    // Don't move string literals
    std::string s2 = std::move("literal");  // Copies
    std::cout << "Moving literal: copies\n";
}

// --- 4. Move and Containers ---

void moveContainersDemo() {
    std::cout << "\n--- Move and Containers ---\n";

    std::vector<ExpensiveResource> vec;
    vec.reserve(3);

    std::cout << "emplace_back (constructs in-place):\n";
    vec.emplace_back("item1");

    std::cout << "\npush_back with lvalue (copies):\n";
    ExpensiveResource item2("item2");
    vec.push_back(item2);

    std::cout << "\npush_back with rvalue (moves):\n";
    vec.push_back(ExpensiveResource("item3"));

    std::cout << "\nstd::move into push_back (moves):\n";
    vec.push_back(std::move(item2));  // item2 is now moved-from

    std::cout << "\nVector size: " << vec.size() << "\n";
}

// --- 5. Move-Only Types ---

class MoveOnly {
public:
    explicit MoveOnly(int value) : value_(value) {
        std::cout << "  [Create] " << value_ << "\n";
    }

    ~MoveOnly() {
        std::cout << "  [Destroy] " << value_ << "\n";
    }

    // Non-copyable
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    // Movable
    MoveOnly(MoveOnly&& other) noexcept : value_(other.value_) {
        other.value_ = 0;
        std::cout << "  [Move] " << value_ << "\n";
    }

    MoveOnly& operator=(MoveOnly&& other) noexcept {
        if (this != &other) {
            value_ = other.value_;
            other.value_ = 0;
            std::cout << "  [Move Assign] " << value_ << "\n";
        }
        return *this;
    }

    int value() const { return value_; }

private:
    int value_;
};

void moveOnlyDemo() {
    std::cout << "\n--- Move-Only Types ---\n";

    MoveOnly a(1);
    // MoveOnly b = a;  // ERROR: deleted copy constructor

    MoveOnly b = std::move(a);  // OK: move constructor
    std::cout << "a.value = " << a.value() << " (moved-from)\n";
    std::cout << "b.value = " << b.value() << "\n";

    MoveOnly c(2);
    c = std::move(b);  // OK: move assignment
    std::cout << "b.value = " << b.value() << " (moved-from)\n";
    std::cout << "c.value = " << c.value() << "\n";
}

// --- 6. Common Pitfalls ---

void commonPitfalls() {
    std::cout << "\n--- Common Pitfalls ---\n";

    // Pitfall 1: Using moved-from object
    std::string s = "hello";
    std::string moved = std::move(s);
    std::cout << "Moved-from string: \"" << s << "\" (valid but unspecified)\n";

    // Pitfall 2: Moving from const
    const std::string cs = "const";
    std::string target = std::move(cs);  // Copies!
    std::cout << "Moving from const: still copies\n";

    // Pitfall 3: Double move
    std::string original = "double move";
    std::string first = std::move(original);
    std::string second = std::move(original);  // original already moved-from
    std::cout << "After double move: \"" << original << "\"\n";
}

// --- 7. Performance Comparison ---

void performanceDemo() {
    std::cout << "\n--- Performance: Copy vs Move ---\n";

    const int iterations = 100000;

    std::vector<std::string> vec;
    vec.reserve(iterations);

    // Copy: slow
    std::string source = std::string(1000, 'x');  // 1KB string
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(source);  // Copies 1KB each time
    }
    std::cout << "Copied 1000 strings (1KB each)\n";

    vec.clear();

    // Move: fast
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(std::string(1000, 'y'));  // Moves (or elided)
    }
    std::cout << "Moved 1000 strings (1KB each)\n";
}

int main() {
    nrvoDemo();
    moveVsCopyDemo();
    whenNotToMove();
    moveContainersDemo();
    moveOnlyDemo();
    commonPitfalls();
    performanceDemo();

    std::cout << "\n=== All scenarios completed ===\n";
    return 0;
}
