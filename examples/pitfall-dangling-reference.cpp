// examples/pitfall-dangling-reference.cpp
// Demonstrates dangling reference pitfalls and their fixes.
// Compile with: g++ -std=c++17 -o pitfall_dangling pitfall-dangling-reference.cpp

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// --- 1. Dangling Reference to Local Variable ---

std::string& badGetGreeting() {
    std::string greeting = "Hello";
    return greeting;  // BAD: returning reference to local
}

// Fix: Return by value
std::string goodGetGreeting() {
    std::string greeting = "Hello";
    return greeting;  // OK: copy elision or move
}

void danglingReferenceToLocal() {
    std::cout << "--- Dangling Reference to Local ---\n";

    // BAD: This is undefined behavior
    // std::string& ref = badGetGreeting();
    // std::cout << "Bad: " << ref << "\n";

    // GOOD: Return by value
    std::string str = goodGetGreeting();
    std::cout << "Good: " << str << "\n";
}

// --- 2. Dangling Reference in Loop ---

void danglingReferenceInLoop() {
    std::cout << "\n--- Dangling Reference in Loop ---\n";

    std::vector<std::string> words = {"hello", "world", "foo", "bar"};

    // BAD: Reference to temporary
    // for (const auto& ref : std::vector<std::string>{"a", "b"}) {
    //     std::cout << ref << "\n";  // ref is dangling after temporary destroyed
    // }

    // GOOD: Use the vector directly
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << "\n";
}

// --- 3. Dangling Reference from Method ---

class BadContainer {
public:
    const std::string& getItem(int index) const {
        std::string temp = "item_" + std::to_string(index);
        return temp;  // BAD: returning reference to local
    }
};

class GoodContainer {
public:
    std::string getItem(int index) const {
        return "item_" + std::to_string(index);  // OK: return by value
    }
};

void danglingReferenceFromMethod() {
    std::cout << "\n--- Dangling Reference from Method ---\n";

    // BAD
    // BadContainer bad;
    // const std::string& item = bad.getItem(0);
    // std::cout << "Bad: " << item << "\n";  // Undefined behavior

    // GOOD
    GoodContainer good;
    std::string item = good.getItem(0);
    std::cout << "Good: " << item << "\n";
}

// --- 4. Dangling Reference with std::string_view ---

void danglingStringView() {
    std::cout << "\n--- Dangling string_view ---\n";

    std::string_view sv;

    {
        std::string s = "Hello, World!";
        sv = s;  // sv views s's data
    }
    // s is destroyed, sv is now dangling

    // BAD
    // std::cout << "Bad: " << sv << "\n";  // Undefined behavior

    // GOOD: Make sure string outlives string_view
    std::string safe = "Safe string";
    std::string_view safeSv = safe;
    std::cout << "Good: " << safeSv << "\n";
}

// --- 5. Dangling Reference in Conditional ---

void danglingReferenceInConditional() {
    std::cout << "\n--- Dangling Reference in Conditional ---\n";

    // BAD
    // const std::string& ref = (true) ? std::string("yes") : std::string("no");
    // std::cout << "Bad: " << ref << "\n";  // ref is dangling

    // GOOD
    std::string value = (true) ? "yes" : "no";
    std::cout << "Good: " << value << "\n";
}

// --- 6. Dangling Reference with Lambda ---

void danglingReferenceWithLambda() {
    std::cout << "\n--- Dangling Reference with Lambda ---\n";

    auto createLambda = []() {
        int local = 42;
        return [&local]() -> int {  // BAD: capturing local by reference
            return local;
        };
    };

    // BAD: lambda captures dangling reference
    // auto func = createLambda();
    // std::cout << "Bad: " << func() << "\n";

    // GOOD: Return by value or use shared_ptr
    auto createSafeLambda = []() {
        auto ptr = std::make_shared<int>(42);
        return [ptr]() -> int {  // OK: shared_ptr extends lifetime
            return *ptr;
        };
    };

    auto safeFunc = createSafeLambda();
    std::cout << "Good: " << safeFunc() << "\n";
}

// --- 7. Fix: Use Smart Pointers ---

void fixWithSmartPointers() {
    std::cout << "\n--- Fix: Use Smart Pointers ---\n";

    // Instead of returning reference, return shared_ptr
    auto createObject = []() {
        return std::make_shared<std::string>("Shared object");
    };

    auto obj = createObject();
    std::cout << "Object: " << *obj << "\n";
    // obj keeps the string alive
}

// --- 8. Fix: Use std::optional ---

std::optional<std::string> findValue(const std::vector<std::string>& vec, int index) {
    if (index >= 0 && index < static_cast<int>(vec.size())) {
        return vec[index];
    }
    return std::nullopt;
}

void fixWithOptional() {
    std::cout << "\n--- Fix: Use std::optional ---\n";

    std::vector<std::string> items = {"a", "b", "c"};

    if (auto value = findValue(items, 1)) {
        std::cout << "Found: " << *value << "\n";
    }

    if (auto value = findValue(items, 10)) {
        std::cout << "Found: " << *value << "\n";
    } else {
        std::cout << "Not found\n";
    }
}

int main() {
    danglingReferenceToLocal();
    danglingReferenceInLoop();
    danglingReferenceFromMethod();
    danglingStringView();
    danglingReferenceInConditional();
    danglingReferenceWithLambda();
    fixWithSmartPointers();
    fixWithOptional();

    std::cout << "\n=== All dangling reference scenarios completed ===\n";
    return 0;
}
