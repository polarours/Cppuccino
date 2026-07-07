// examples/cpp20-ranges.cpp
// Demonstrates C++20 ranges library.
// Compile with: g++ -std=c++20 -o cpp20_ranges cpp20-ranges.cpp

#include <iostream>
#include <ranges>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

// --- 1. Basic Range Operations ---

void basicRangeOperations() {
    std::cout << "--- Basic Range Operations ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Filter even numbers
    auto even = [](int n) { return n % 2 == 0; };
    std::cout << "Even: ";
    for (int n : nums | std::views::filter(even)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // Transform (square)
    std::cout << "Squared: ";
    for (int n : nums | std::views::transform([](int n) { return n * n; })) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 2. Chaining Operations ---

void chainingOperations() {
    std::cout << "\n--- Chaining Operations ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Filter even, take first 3
    std::cout << "First 3 even: ";
    for (int n : nums
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::take(3)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // Transform, filter, collect
    auto result = nums
        | std::views::transform([](int n) { return n * 10; })
        | std::views::filter([](int n) { return n > 30; });

    std::cout << "Multiply by 10, filter > 30: ";
    for (int n : result) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 3. Range Adaptors ---

void rangeAdaptors() {
    std::cout << "\n--- Range Adaptors ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // take, drop
    std::cout << "Take 5: ";
    for (int n : nums | std::views::take(5)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    std::cout << "Drop 5: ";
    for (int n : nums | std::views::drop(5)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // reverse
    std::cout << "Reverse: ";
    for (int n : nums | std::views::reverse) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // take_while
    std::cout << "Take while < 5: ";
    for (int n : nums | std::views::take_while([](int n) { return n < 5; })) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 4. Views ---

void views() {
    std::cout << "\n--- Views ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    // iota (generate sequence)
    std::cout << "iota(1, 6): ";
    for (int n : std::views::iota(1, 6)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // filter
    std::cout << "Filter > 3: ";
    for (int n : nums | std::views::filter([](int n) { return n > 3; })) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // transform
    std::cout << "Transform (x2): ";
    for (int n : nums | std::views::transform([](int n) { return n * 2; })) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // elements (for tuple-like)
    std::vector<std::pair<int, std::string>> pairs = {
        {1, "one"}, {2, "two"}, {3, "three"}
    };
    std::cout << "Keys: ";
    for (int key : pairs | std::views::keys) {
        std::cout << key << " ";
    }
    std::cout << "\n";
}

// --- 5. String Operations ---

void stringOperations() {
    std::cout << "\n--- String Operations ---\n";

    std::string text = "Hello, World!";

    // Split by comma
    std::cout << "Words: ";
    for (auto word : text | std::views::split(',')) {
        for (char c : word) {
            std::cout << c;
        }
        std::cout << " ";
    }
    std::cout << "\n";

    // Join
    std::vector<std::string> words = {"Hello", "World", "C++"};
    std::cout << "Joined: ";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << "\n";
}

// --- 6. Zip View (C++23 preview) ---

void zipView() {
    std::cout << "\n--- Zip-like Pattern ---\n";

    std::vector<int> nums = {1, 2, 3};
    std::vector<std::string> names = {"one", "two", "three"};

    // Manual zip using indices
    for (size_t i = 0; i < nums.size(); ++i) {
        std::cout << nums[i] << ": " << names[i] << "\n";
    }
}

// --- 7. Range Algorithms ---

void rangeAlgorithms() {
    std::cout << "\n--- Range Algorithms ---\n";

    std::vector<int> nums = {5, 3, 1, 4, 2};

    // sort using ranges
    std::ranges::sort(nums);
    std::cout << "Sorted: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";

    // find
    auto it = std::ranges::find(nums, 3);
    if (it != nums.end()) {
        std::cout << "Found 3 at index: " << std::distance(nums.begin(), it) << "\n";
    }

    // count
    auto count = std::ranges::count_if(nums, [](int n) { return n > 2; });
    std::cout << "Count > 2: " << count << "\n";
}

// --- 8. Lazy Evaluation ---

void lazyEvaluation() {
    std::cout << "\n--- Lazy Evaluation ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Operations are lazy
    auto lazy = nums
        | std::views::filter([](int n) {
            std::cout << "  filter(" << n << ") ";
            return n % 2 == 0;
        })
        | std::views::transform([](int n) {
            std::cout << "transform(" << n << ") ";
            return n * n;
        });

    std::cout << "Result: ";
    for (int n : lazy) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 9. Composing Views ---

void composingViews() {
    std::cout << "\n--- Composing Views ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto result = nums
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; })
        | std::views::take(3);

    std::cout << "First 3 even squares: ";
    for (int n : result) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 10. Practical Example ---

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    struct Student {
        std::string name;
        int grade;
    };

    std::vector<Student> students = {
        {"Alice", 85},
        {"Bob", 92},
        {"Charlie", 78},
        {"Diana", 95},
        {"Eve", 88}
    };

    // Get names of students with grade > 85
    std::cout << "Top students (grade > 85):\n";
    for (const auto& name : students
        | std::views::filter([](const Student& s) { return s.grade > 85; })
        | std::views::transform([](const Student& s) { return s.name; })) {
        std::cout << "  " << name << "\n";
    }

    // Calculate average grade
    double sum = std::accumulate(students.begin(), students.end(), 0.0,
        [](double acc, const Student& s) { return acc + s.grade; });
    std::cout << "Average grade: " << sum / students.size() << "\n";
}

int main() {
    basicRangeOperations();
    chainingOperations();
    rangeAdaptors();
    views();
    stringOperations();
    zipView();
    rangeAlgorithms();
    lazyEvaluation();
    composingViews();
    practicalExample();

    std::cout << "\n=== All ranges scenarios completed ===\n";
    return 0;
}
