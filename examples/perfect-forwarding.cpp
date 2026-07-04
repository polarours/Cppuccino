// examples/perfect_forwarding.cpp
// Demonstrates forwarding references and perfect forwarding.
// Compile with: g++ -std=c++17 -o perfect_forwarding perfect-forwarding.cpp

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// --- 1. The Problem: Why Perfect Forwarding? ---

class Widget {
public:
    Widget() { std::cout << "Default constructed\n"; }
    Widget(const Widget&) { std::cout << "Copy constructed\n"; }
    Widget(Widget&&) noexcept { std::cout << "Move constructed\n"; }
};

// Bad: Cannot forward both lvalues and rvalues
void badWrapper(const Widget& w) {
    std::cout << "Bad wrapper (const lvalue ref): ";
    Widget local = w;  // Always copies!
}

void badWrapper(Widget&& w) {
    std::cout << "Bad wrapper (rvalue ref): ";
    Widget local = std::move(w);  // Always moves!
}

// Problem: We need overloads for every combination
// And it doesn't scale with multiple parameters

// --- 2. The Solution: Forwarding References ---

template <typename T>
void goodWrapper(T&& w) {
    std::cout << "Good wrapper: ";
    // T&& is a forwarding reference when T is deduced
    Widget local(std::forward<T>(w));  // Preserves value category
}

// --- 3. Forwarding References Explained ---

void forwardingRefDemo() {
    std::cout << "--- Forwarding References ---\n";

    Widget w;

    std::cout << "Calling with lvalue:\n";
    goodWrapper(w);  // T = Widget&, so T&& = Widget& (lvalue ref)

    std::cout << "Calling with rvalue:\n";
    goodWrapper(Widget());  // T = Widget, so T&& = Widget&& (rvalue ref)
}

// --- 4. Perfect Forwarding with Multiple Parameters ---

class MultiWidget {
public:
    MultiWidget(int id, std::string name, double value)
        : id_(id), name_(std::move(name)), value_(value) {
        std::cout << "MultiWidget(" << id_ << ", " << name_ << ", " << value_ << ")\n";
    }

private:
    int id_;
    std::string name_;
    double value_;
};

// Factory function using perfect forwarding
template <typename... Args>
MultiWidget createMultiWidget(Args&&... args) {
    std::cout << "Factory: forwarding " << sizeof...(args) << " args\n";
    return MultiWidget(std::forward<Args>(args)...);
}

void multiParamDemo() {
    std::cout << "\n--- Multiple Parameters ---\n";

    auto w1 = createMultiWidget(1, std::string("Alice"), 3.14);
    auto w2 = createMultiWidget(2, "Bob", 2.71);  // const char* forwarded
}

// --- 5. Perfect Forwarding in Containers ---

void containerDemo() {
    std::cout << "\n--- Container Forwarding ---\n";

    std::vector<Widget> widgets;

    std::cout << "push_back lvalue:\n";
    Widget w;
    widgets.push_back(w);  // Copy

    std::cout << "push_back rvalue:\n";
    widgets.push_back(Widget());  // Move

    std::cout << "emplace_back (forwards args to constructor):\n";
    widgets.emplace_back();  // Constructs in-place
}

// --- 6. std::forward vs std::move ---

void forwardVsMoveDemo() {
    std::cout << "\n--- std::forward vs std::move ---\n";

    std::cout << "std::move: Unconditionally casts to rvalue\n";
    std::cout << "std::forward<T>: Conditionally casts based on T\n";

    Widget w;

    // std::move is equivalent to:
    // static_cast<std::remove_reference_t<Widget>&&>(w)

    // std::forward<Widget&>(w) is equivalent to:
    // static_cast<Widget&>(w)  (preserves lvalue)

    // std::forward<Widget>(w) is equivalent to:
    // static_cast<Widget&&>(w)  (converts to rvalue)
}

// --- 7. Common Mistake: Forgetting std::forward ---

template <typename T>
void forgottenForward(T&& value) {
    std::cout << "Without forward: ";
    Widget local(value);  // Always copies! T&& collapsed to lvalue ref
}

template <typename T>
void correctForward(T&& value) {
    std::cout << "With forward: ";
    Widget local(std::forward<T>(value));  // Correctly forwards
}

void mistakeDemo() {
    std::cout << "\n--- Common Mistake ---\n";

    std::cout << "Calling with rvalue:\n";
    forgottenForward(Widget());   // Copies (bad!)
    correctForward(Widget());     // Moves (good!)
}

// --- 8. Forwarding References in Class Templates ---

template <typename T>
class Container {
public:
    void push(T&& value) {
        std::cout << "push: ";
        data_.push_back(std::forward<T>(value));
    }

    void push(const T& value) {
        std::cout << "push (const): ";
        data_.push_back(value);
    }

private:
    std::vector<T> data_;
};

void classTemplateDemo() {
    std::cout << "\n--- Forwarding in Class Templates ---\n";

    Container<Widget> container;

    Widget w;
    container.push(w);           // Lvalue: copies
    container.push(Widget());    // Rvalue: moves
}

// --- 9. Real-World Example: std::make_unique ---

// Simplified version of std::make_unique
template <typename T, typename... Args>
std::unique_ptr<T> makeUnique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

void realWorldDemo() {
    std::cout << "\n--- Real-World: make_unique ---\n";

    auto w1 = makeUnique<MultiWidget>(1, std::string("Test"), 1.0);
    auto w2 = makeUnique<MultiWidget>(2, "Test2", 2.0);
}

int main() {
    forwardingRefDemo();
    multiParamDemo();
    containerDemo();
    forwardVsMoveDemo();
    mistakeDemo();
    classTemplateDemo();
    realWorldDemo();

    std::cout << "\n=== All scenarios completed ===\n";
    return 0;
}
