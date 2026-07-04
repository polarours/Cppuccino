// examples/smart_pointers_ownership.cpp
// Demonstrates unique_ptr, shared_ptr, weak_ptr, and ownership patterns.
// Compile with: g++ -std=c++17 -o smart_pointers smart-pointers-ownership.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// --- 1. Resource class for demonstration ---

class Resource {
public:
    explicit Resource(std::string name) : name_(std::move(name)) {
        std::cout << "[Acquire] " << name_ << "\n";
    }

    ~Resource() {
        std::cout << "[Release] " << name_ << "\n";
    }

    const std::string& name() const { return name_; }

private:
    std::string name_;
};

// --- 2. unique_ptr: Exclusive Ownership ---

void uniquePtrDemo() {
    std::cout << "--- unique_ptr: Exclusive Ownership ---\n";

    // Creation
    auto res = std::make_unique<Resource>("unique_1");
    std::cout << "Owner: " << res->name() << "\n";

    // Cannot copy
    // auto copy = res;  // ERROR: copy constructor deleted

    // Can move
    auto moved = std::move(res);
    std::cout << "After move, original is " << (res ? "valid" : "empty") << "\n";
    std::cout << "Moved to: " << moved->name() << "\n";

    // Release ownership
    Resource* raw = moved.release();
    std::cout << "After release, unique_ptr is " << (moved ? "valid" : "empty") << "\n";
    delete raw;  // Manual cleanup required!

    // Reset
    auto another = std::make_unique<Resource>("unique_2");
    another.reset();  // Destroys the resource
    std::cout << "After reset: " << (another ? "valid" : "empty") << "\n";
}

// --- 3. shared_ptr: Shared Ownership ---

void sharedPtrDemo() {
    std::cout << "\n--- shared_ptr: Shared Ownership ---\n";

    auto res = std::make_shared<Resource>("shared_1");
    std::cout << "use_count = " << res.use_count() << "\n";

    {
        auto copy1 = res;
        auto copy2 = res;
        std::cout << "After 2 copies, use_count = " << res.use_count() << "\n";

        copy1->name();  // All owners can use the resource
    }
    std::cout << "After scope, use_count = " << res.use_count() << "\n";
}

// --- 4. shared_ptr Control Block ---

void controlBlockDemo() {
    std::cout << "\n--- Control Block ---\n";

    auto res = std::make_shared<Resource>("control_block");
    std::cout << "Pointer value: " << res.get() << "\n";
    std::cout << "use_count = " << res.use_count() << "\n";

    // Control block stores:
    // - Strong reference count
    // - Weak reference count
    // - Deleter (if custom)
    // - Allocator (if custom)
}

// --- 5. weak_ptr: Breaking Cycles ---

class Person {
public:
    Person(std::string name) : name_(std::move(name)) {
        std::cout << "[Create] " << name_ << "\n";
    }

    ~Person() {
        std::cout << "[Destroy] " << name_ << "\n";
    }

    void setParent(std::shared_ptr<Person> parent) {
        parent_ = parent;
    }

    void printParent() const {
        if (auto p = parent_.lock()) {
            std::cout << name_ << "'s parent: " << p->name_ << "\n";
        } else {
            std::cout << name_ << "'s parent: (expired)\n";
        }
    }

private:
    std::string name_;
    std::weak_ptr<Person> parent_;  // Weak reference to avoid cycle
};

void weakPtrDemo() {
    std::cout << "\n--- weak_ptr: Breaking Cycles ---\n";

    auto parent = std::make_shared<Person>("Alice");
    auto child = std::make_shared<Person>("Bob");

    child->setParent(parent);
    child->printParent();

    parent.reset();  // Destroy parent
    std::cout << "After parent.reset():\n";
    child->printParent();  // weak_ptr detects expired resource
}

// --- 6. Circular Reference Problem ---

class Node {
public:
    Node(std::string name) : name_(std::move(name)) {
        std::cout << "[Create] Node " << name_ << "\n";
    }

    ~Node() {
        std::cout << "[Destroy] Node " << name_ << "\n";
    }

    std::shared_ptr<Node> next;  // Strong reference to next node

private:
    std::string name_;
};

void circularReferenceProblem() {
    std::cout << "\n--- Circular Reference Problem ---\n";
    std::cout << "Creating cycle: A -> B -> A\n";

    auto a = std::make_shared<Node>("A");
    auto b = std::make_shared<Node>("B");

    a->next = b;  // A -> B
    b->next = a;  // B -> A (CYCLE!)

    std::cout << "use_count(a) = " << a.use_count() << "\n";
    std::cout << "use_count(b) = " << b.use_count() << "\n";

    // When a and b go out of scope:
    // - use_count drops to 1 for each (still held by the other)
    // - Memory is never freed! (LEAK)
    std::cout << "WARNING: Memory leak will occur!\n";
}

// --- 7. Breaking the Cycle with weak_ptr ---

class SafeNode {
public:
    SafeNode(std::string name) : name_(std::move(name)) {
        std::cout << "[Create] SafeNode " << name_ << "\n";
    }

    ~SafeNode() {
        std::cout << "[Destroy] SafeNode " << name_ << "\n";
    }

    std::shared_ptr<SafeNode> next;    // Strong reference
    std::weak_ptr<SafeNode> previous;  // Weak reference (breaks cycle)

private:
    std::string name_;
};

void breakCycleWithWeakPtr() {
    std::cout << "\n--- Breaking Cycle with weak_ptr ---\n";

    auto a = std::make_shared<SafeNode>("A");
    auto b = std::make_shared<SafeNode>("B");

    a->next = b;      // A -> B (strong)
    b->previous = a;  // B -> A (weak)

    std::cout << "use_count(a) = " << a.use_count() << "\n";
    std::cout << "use_count(b) = " << b.use_count() << "\n";

    // No leak: weak_ptr doesn't prevent destruction
}

// --- 8. Custom Deleter ---

void customDeleterDemo() {
    std::cout << "\n--- Custom Deleter ---\n";

    auto deleter = [](Resource* r) {
        std::cout << "[Custom Delete] " << r->name() << "\n";
        delete r;
    };

    {
        std::unique_ptr<Resource, decltype(deleter)> res(
            new Resource("custom_deleter"), deleter
        );
    }
    // Custom deleter called when res goes out of scope
}

// --- 9. make_shared vs new ---

void makeSharedVsNew() {
    std::cout << "\n--- make_shared vs new ---\n";

    // make_shared: Single allocation (object + control block)
    auto efficient = std::make_shared<Resource>("efficient");

    // new + shared_ptr: Two allocations
    auto less_efficient = std::shared_ptr<Resource>(new Resource("less_efficient"));

    std::cout << "make_shared is more efficient (1 allocation vs 2)\n";
}

int main() {
    uniquePtrDemo();
    sharedPtrDemo();
    controlBlockDemo();
    weakPtrDemo();
    breakCycleWithWeakPtr();
    customDeleterDemo();
    makeSharedVsNew();

    std::cout << "\n=== All scenarios completed ===\n";
    return 0;
}
