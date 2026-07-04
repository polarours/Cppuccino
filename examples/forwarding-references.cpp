// examples/forwarding_references.cpp
// Demonstrates forwarding references and perfect forwarding in C++.
// Compile with: g++ -std=c++17 -o forwarding forwarding_references.cpp

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <memory>

// Perfect forwarding wrapper
template<typename T>
void wrapper(T&& arg) {
    // std::forward preserves value category
    process(std::forward<T>(arg));
}

// Overloaded functions to demonstrate forwarding
void process(int& x) {
    std::cout << "process(int&): " << x << '\n';
    x += 10;
}

void process(const int& x) {
    std::cout << "process(const int&): " << x << '\n';
}

void process(int&& x) {
    std::cout << "process(int&&): " << x << '\n';
    x += 100;
}

// Factory function using perfect forwarding
template<typename T, typename... Args>
std::unique_ptr<T> create(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// Container with perfect forwarding push
template<typename T>
class Container {
public:
    template<typename U>
    void push(U&& value) {
        items_.push_back(std::forward<U>(value));
        std::cout << "Pushed: " << value << '\n';
    }
    
    void print() const {
        for (const auto& item : items_) {
            std::cout << item << ' ';
        }
        std::cout << '\n';
    }

private:
    std::vector<T> items_;
};

// Demonstrate forwarding with different value categories
void demonstrateForwarding() {
    std::cout << "=== Perfect Forwarding Demo ===\n";
    
    int x = 42;
    const int cx = 100;
    
    std::cout << "\n--- Direct calls ---\n";
    process(x);         // lvalue -> process(int&)
    process(cx);        // const lvalue -> process(const int&)
    process(777);       // rvalue -> process(int&&)
    
    std::cout << "\n--- Through wrapper (preserves value category) ---\n";
    wrapper(x);         // forwards as lvalue
    wrapper(cx);        // forwards as const lvalue
    wrapper(888);       // forwards as rvalue
    
    std::cout << "\n--- After modification ---\n";
    std::cout << "x = " << x << '\n';  // Modified by process(int&)
}

// Factory pattern with perfect forwarding
class Widget {
public:
    Widget(const std::string& name, int value) 
        : name_(name), value_(value) {
        std::cout << "Widget constructed: " << name_ << " = " << value_ << '\n';
    }
    
    Widget(std::string&& name, int value) 
        : name_(std::move(name)), value_(value) {
        std::cout << "Widget constructed (moved name): " << name_ << " = " << value_ << '\n';
    }
    
    void print() const {
        std::cout << "Widget: " << name_ << " = " << value_ << '\n';
    }

private:
    std::string name_;
    int value_;
};

int main() {
    demonstrateForwarding();
    
    std::cout << "\n=== Factory Pattern Demo ===\n";
    auto w1 = create<Widget>("Hello", 42);
    w1->print();
    
    std::string name = "World";
    auto w2 = create<Widget>(name, 100);
    w2->print();
    
    auto w3 = create<Widget>(std::string("Moved"), 200);
    w3->print();
    
    std::cout << "\n=== Container with Forwarding ===\n";
    Container<std::string> strs;
    std::string s1 = "Hello";
    strs.push(s1);           // lvalue
    strs.push("World");      // const char* -> rvalue
    strs.push(std::string("Moved"));  // rvalue
    strs.print();
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
