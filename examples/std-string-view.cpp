// examples/std-string-view.cpp
// Demonstrates std::string_view for non-owning string references.
// Compile with: g++ -std=c++17 -o std_string_view std-string-view.cpp

#include <iostream>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>

// --- 1. Basic Usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    std::string_view sv = "Hello, World!";
    std::cout << "View: " << sv << "\n";
    std::cout << "Size: " << sv.size() << "\n";
    std::cout << "First char: " << sv[0] << "\n";
}

// --- 2. From string ---

void fromString() {
    std::cout << "\n--- From String ---\n";

    std::string str = "Hello";
    std::string_view sv = str;

    std::cout << "String: " << str << "\n";
    std::cout << "View: " << sv << "\n";

    // View doesn't own the data
    str += " World";
    std::cout << "After modification:\n";
    std::cout << "String: " << str << "\n";
    std::cout << "View: " << sv << "\n";  // Still points to original data
}

// --- 3. From literal ---

void fromLiteral() {
    std::cout << "\n--- From Literal ---\n";

    std::string_view sv = "Hello";
    std::cout << "View: " << sv << "\n";
    std::cout << "Size: " << sv.size() << "\n";
}

// --- 4. Substrings ---

void substrings() {
    std::cout << "\n--- Substrings ---\n";

    std::string_view sv = "Hello, World!";

    std::cout << "substr(0, 5): " << sv.substr(0, 5) << "\n";
    std::cout << "substr(7): " << sv.substr(7) << "\n";
}

// --- 5. Search operations ---

void searchOperations() {
    std::cout << "\n--- Search Operations ---\n";

    std::string_view sv = "Hello, World!";

    std::cout << "find('o'): " << sv.find('o') << "\n";
    std::cout << "find('o', 5): " << sv.find('o', 5) << "\n";
    std::cout << "rfind('o'): " << sv.rfind('o') << "\n";
    std::cout << "find(\"World\"): " << sv.find("World") << "\n";
}

// --- 6. Comparison ---

void comparison() {
    std::cout << "\n--- Comparison ---\n";

    std::string_view sv1 = "Hello";
    std::string_view sv2 = "World";
    std::string_view sv3 = "Hello";

    std::cout << "sv1 == sv3: " << (sv1 == sv3) << "\n";
    std::cout << "sv1 < sv2: " << (sv1 < sv2) << "\n";
    std::cout << "sv1.compare(sv2): " << sv1.compare(sv2) << "\n";
}

// --- 7. Modifiers ---

void modifiers() {
    std::cout << "\n--- Modifiers ---\n";

    std::string_view sv = "Hello, World!";

    sv.remove_prefix(7);
    std::cout << "After remove_prefix(7): " << sv << "\n";

    sv.remove_suffix(1);
    std::cout << "After remove_suffix(1): " << sv << "\n";
}

// --- 8. With functions ---

std::string_view getGreeting() {
    return "Hello from function";
}

int countWords(std::string_view text) {
    int count = 0;
    bool inWord = false;

    for (char c : text) {
        if (std::isspace(c)) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            count++;
        }
    }
    return count;
}

void withFunctions() {
    std::cout << "\n--- With Functions ---\n";

    std::cout << getGreeting() << "\n";
    std::cout << "Words in 'Hello World': " << countWords("Hello World") << "\n";
    std::cout << "Words in '  spaces  ': " << countWords("  spaces  ") << "\n";
}

// --- 9. With containers ---

void withContainers() {
    std::cout << "\n--- With Containers ---\n";

    std::vector<std::string_view> words = {"Hello", "World", "C++", "17"};

    for (std::string_view word : words) {
        std::cout << word << " ";
    }
    std::cout << "\n";
}

// --- 10. Performance benefit ---

void performanceBenefit() {
    std::cout << "\n--- Performance Benefit ---\n";

    // Without string_view: creates copies
    auto processString = [](const std::string& s) {
        std::cout << "string: " << s.substr(0, 5) << "\n";
    };

    // With string_view: no copies
    auto processView = [](std::string_view sv) {
        std::cout << "string_view: " << sv.substr(0, 5) << "\n";
    };

    std::string str = "Hello, World!";
    processString(str);  // Substring creates a copy
    processView(str);    // No copy
}

// --- 11. Null termination ---

void nullTermination() {
    std::cout << "\n--- Null Termination ---\n";

    std::string_view sv = "Hello";
    std::cout << "data(): " << sv.data() << "\n";
    std::cout << "size(): " << sv.size() << "\n";

    // Note: string_view may not be null-terminated
    // Use with caution when passing to C APIs
}

// --- 12. Practical example ---

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    // Parse simple key=value pairs
    auto parsePair = [](std::string_view input) -> std::pair<std::string_view, std::string_view> {
        size_t eq = input.find('=');
        if (eq == std::string_view::npos) {
            return {"", ""};
        }
        return {input.substr(0, eq), input.substr(eq + 1)};
    };

    std::string_view data = "name=Alice;age=25;city=NYC";
    size_t pos = 0;

    while (pos < data.size()) {
        size_t end = data.find(';', pos);
        if (end == std::string_view::npos) end = data.size();

        auto [key, value] = parsePair(data.substr(pos, end - pos));
        if (!key.empty()) {
            std::cout << key << " = " << value << "\n";
        }
        pos = end + 1;
    }
}

int main() {
    basicUsage();
    fromString();
    fromLiteral();
    substrings();
    searchOperations();
    comparison();
    modifiers();
    withFunctions();
    withContainers();
    performanceBenefit();
    nullTermination();
    practicalExample();

    std::cout << "\n=== All string_view scenarios completed ===\n";
    return 0;
}
