// examples/range-based-for.cpp
// Demonstrates range-based for loops with various containers.
// Compile with: g++ -std=c++17 -o range_based_for range-based-for.cpp

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <string>
#include <list>
#include <unordered_map>

// --- 1. Basic Usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    // By value (copy)
    std::cout << "By value: ";
    for (int n : nums) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // By reference (no copy, read-only)
    std::cout << "By const ref: ";
    for (const int& n : nums) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // By reference (modifiable)
    std::vector<int> modifiable = {1, 2, 3};
    for (int& n : modifiable) {
        n *= 2;
    }
    std::cout << "Modified: ";
    for (const int& n : modifiable) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 2. With Different Containers ---

void differentContainers() {
    std::cout << "\n--- Different Containers ---\n";

    // std::array
    std::array<int, 4> arr = {10, 20, 30, 40};
    std::cout << "array: ";
    for (const auto& val : arr) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    // std::list
    std::list<std::string> names = {"Alice", "Bob", "Charlie"};
    std::cout << "list: ";
    for (const auto& name : names) {
        std::cout << name << " ";
    }
    std::cout << "\n";

    // std::set
    std::set<int> unique = {5, 3, 1, 4, 2};
    std::cout << "set: ";
    for (int val : unique) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

// --- 3. Map Iteration ---

void mapIteration() {
    std::cout << "\n--- Map Iteration ---\n";

    std::map<std::string, int> ages = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 35}
    };

    // Structured bindings (C++17)
    std::cout << "Structured bindings:\n";
    for (const auto& [name, age] : ages) {
        std::cout << "  " << name << ": " << age << "\n";
    }

    // Iterating over pairs
    std::cout << "Using pair:\n";
    for (const auto& pair : ages) {
        std::cout << "  " << pair.first << ": " << pair.second << "\n";
    }
}

// --- 4. String as Range ---

void stringAsRange() {
    std::cout << "\n--- String as Range ---\n";

    std::string text = "Hello, World!";

    std::cout << "Characters: ";
    for (char c : text) {
        std::cout << c << " ";
    }
    std::cout << "\n";

    // Modifying characters
    std::string upper = text;
    for (char& c : upper) {
        c = std::toupper(c);
    }
    std::cout << "Uppercase: " << upper << "\n";
}

// --- 5. Initializer List ---

void initializerList() {
    std::cout << "\n--- Initializer List ---\n";

    std::cout << "Numbers: ";
    for (int n : {1, 2, 3, 4, 5}) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    std::cout << "Strings: ";
    for (const auto& s : {"hello", "world", "!"}) {
        std::cout << s << " ";
    }
    std::cout << "\n";
}

// --- 6. With Algorithms ---

void withAlgorithms() {
    std::cout << "\n--- With Algorithms ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Count even numbers
    int evenCount = 0;
    for (int n : nums) {
        if (n % 2 == 0) {
            evenCount++;
        }
    }
    std::cout << "Even numbers: " << evenCount << "\n";

    // Find first greater than 5
    int found = 0;
    for (int n : nums) {
        if (n > 5) {
            found = n;
            break;
        }
    }
    std::cout << "First > 5: " << found << "\n";
}

// --- 7. Nested Loops ---

void nestedLoops() {
    std::cout << "\n--- Nested Loops ---\n";

    std::vector<std::vector<int>> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    std::cout << "Matrix:\n";
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

// --- 8. Custom Range ---

class IntRange {
public:
    IntRange(int start, int end) : start_(start), end_(end) {}

    int* begin() { return &start_; }
    int* end() { return &end_; }

private:
    int start_;
    int end_;
};

class FibonacciRange {
public:
    FibonacciRange(int count) : count_(count) {}

    class Iterator {
    public:
        Iterator(int count, int a, int b)
            : count_(count), a_(a), b_(b) {}

        int operator*() const { return a_; }

        Iterator& operator++() {
            int next = a_ + b_;
            a_ = b_;
            b_ = next;
            count_--;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return count_ != other.count_;
        }

    private:
        int count_;
        int a_, b_;
    };

    Iterator begin() { return Iterator(count_, 0, 1); }
    Iterator end() { return Iterator(0, 0, 0); }

private:
    int count_;
};

void customRange() {
    std::cout << "\n--- Custom Range ---\n";

    std::cout << "IntRange: ";
    for (int n : IntRange(1, 5)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    std::cout << "Fibonacci: ";
    for (int n : FibonacciRange(10)) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 9. C++20 Ranges (preview) ---

void cpp20Ranges() {
    std::cout << "\n--- C++20 Ranges (conceptual) ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Filter even numbers
    std::cout << "Even: ";
    for (int n : nums) {
        if (n % 2 == 0) {
            std::cout << n << " ";
        }
    }
    std::cout << "\n";

    // Transform (square)
    std::cout << "Squared: ";
    for (int n : nums) {
        std::cout << n * n << " ";
    }
    std::cout << "\n";
}

int main() {
    basicUsage();
    differentContainers();
    mapIteration();
    stringAsRange();
    initializerList();
    withAlgorithms();
    nestedLoops();
    customRange();
    cpp20Ranges();

    std::cout << "\n=== All range-based-for scenarios completed ===\n";
    return 0;
}
