// examples/std-any.cpp
// Demonstrates std::any for type-erased containers.
// Compile with: g++ -std=c++17 -o std_any std-any.cpp

#include <iostream>
#include <any>
#include <string>
#include <vector>
#include <map>

// --- 1. Basic Usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    std::any a;

    a = 42;
    std::cout << "int: " << std::any_cast<int>(a) << "\n";

    a = 3.14;
    std::cout << "double: " << std::any_cast<double>(a) << "\n";

    a = std::string("hello");
    std::cout << "string: " << std::any_cast<std::string>(a) << "\n";
}

// --- 2. Type checking ---

void typeChecking() {
    std::cout << "\n--- Type Checking ---\n";

    std::any a = 42;

    std::cout << "type: " << a.type().name() << "\n";
    std::cout << "has value: " << a.has_value() << "\n";

    a.reset();
    std::cout << "after reset: " << a.has_value() << "\n";
}

// --- 3. Safe casting ---

void safeCasting() {
    std::cout << "\n--- Safe Casting ---\n";

    std::any a = 42;

    try {
        double d = std::any_cast<double>(a);
        std::cout << "double: " << d << "\n";
    } catch (const std::bad_any_cast& e) {
        std::cout << "Error: " << e.what() << "\n";
    }

    // Use any_cast with pointer for safe access
    if (auto* p = std::any_cast<int>(&a)) {
        std::cout << "int: " << *p << "\n";
    }
}

// --- 4. Any with custom types ---

struct Person {
    std::string name;
    int age;
};

void customTypes() {
    std::cout << "\n--- Custom Types ---\n";

    std::any a = Person{"Alice", 25};
    const auto& person = std::any_cast<const Person&>(a);

    std::cout << "Name: " << person.name << "\n";
    std::cout << "Age: " << person.age << "\n";
}

// --- 5. Any in container ---

void anyInContainer() {
    std::cout << "\n--- Any in Container ---\n";

    std::vector<std::any> values = {42, 3.14, std::string("hello"), true};

    for (const auto& v : values) {
        std::cout << "Type: " << v.type().name() << ", ";
        if (v.type() == typeid(int)) {
            std::cout << "Value: " << std::any_cast<int>(v);
        } else if (v.type() == typeid(double)) {
            std::cout << "Value: " << std::any_cast<double>(v);
        } else if (v.type() == typeid(std::string)) {
            std::cout << "Value: " << std::any_cast<std::string>(v);
        } else if (v.type() == typeid(bool)) {
            std::cout << "Value: " << std::boolalpha << std::any_cast<bool>(v);
        }
        std::cout << "\n";
    }
}

// --- 6. Any in map ---

void anyInMap() {
    std::cout << "\n--- Any in Map ---\n";

    std::map<std::string, std::any> config;
    config["timeout"] = 30;
    config["host"] = std::string("localhost");
    config["debug"] = true;

    std::cout << "timeout: " << std::any_cast<int>(config["timeout"]) << "\n";
    std::cout << "host: " << std::any_cast<std::string>(config["host"]) << "\n";
    std::cout << "debug: " << std::boolalpha << std::any_cast<bool>(config["debug"]) << "\n";
}

// --- 7. Any with move semantics ---

void moveSemantics() {
    std::cout << "\n--- Move Semantics ---\n";

    std::any a = std::string("hello");

    // Move string out of any
    std::string moved = std::any_cast<std::string&&>(std::move(a));

    std::cout << "Moved: " << moved << "\n";
    std::cout << "Original has value: " << a.has_value() << "\n";
}

// --- 8. Any reset and reassign ---

void resetReassign() {
    std::cout << "\n--- Reset and Reassign ---\n";

    std::any a = 42;
    std::cout << "First: " << std::any_cast<int>(a) << "\n";

    a = std::string("new value");
    std::cout << "After reassign: " << std::any_cast<std::string>(a) << "\n";

    a.reset();
    std::cout << "After reset: " << a.has_value() << "\n";
}

// --- 9. Any comparison ---

void comparison() {
    std::cout << "\n--- Comparison ---\n";

    std::any a = 42;
    std::any b = 42;
    std::any c = 43;

    std::cout << "a == b: " << (std::any_cast<int>(a) == std::any_cast<int>(b)) << "\n";
    std::cout << "a == c: " << (std::any_cast<int>(a) == std::any_cast<int>(c)) << "\n";
}

// --- 10. Practical example ---

class PropertyBag {
public:
    template <typename T>
    void set(const std::string& key, T value) {
        properties_[key] = std::move(value);
    }

    template <typename T>
    T get(const std::string& key) const {
        return std::any_cast<T>(properties_.at(key));
    }

    bool has(const std::string& key) const {
        return properties_.find(key) != properties_.end();
    }

private:
    std::map<std::string, std::any> properties_;
};

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    PropertyBag bag;
    bag.set("name", std::string("Alice"));
    bag.set("age", 25);
    bag.set("score", 95.5);

    std::cout << "name: " << bag.get<std::string>("name") << "\n";
    std::cout << "age: " << bag.get<int>("age") << "\n";
    std::cout << "score: " << bag.get<double>("score") << "\n";
}

int main() {
    basicUsage();
    typeChecking();
    safeCasting();
    customTypes();
    anyInContainer();
    anyInMap();
    moveSemantics();
    resetReassign();
    comparison();
    practicalExample();

    std::cout << "\n=== All any scenarios completed ===\n";
    return 0;
}
