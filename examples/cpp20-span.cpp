// examples/cpp20-span.cpp
// Demonstrates C++20 std::span for non-owning views.
// Compile with: g++ -std=c++20 -o cpp20_span cpp20-span.cpp

#include <iostream>
#include <span>
#include <vector>
#include <array>
#include <cstring>
#include <algorithm>

// --- 1. Basic Usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    int arr[] = {1, 2, 3, 4, 5};
    std::span<int> span1(arr, 5);

    std::cout << "Span from array: ";
    for (int n : span1) std::cout << n << " ";
    std::cout << "\n";

    // From vector
    std::vector<int> vec = {10, 20, 30};
    std::span<int> span2(vec);

    std::cout << "Span from vector: ";
    for (int n : span2) std::cout << n << " ";
    std::cout << "\n";
}

// --- 2. Subspan ---

void subspan() {
    std::cout << "\n--- Subspan ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8};
    std::span<int> span(vec);

    // First 3 elements
    std::cout << "First 3: ";
    for (int n : span.first(3)) std::cout << n << " ";
    std::cout << "\n";

    // Last 3 elements
    std::cout << "Last 3: ";
    for (int n : span.last(3)) std::cout << n << " ";
    std::cout << "\n";

    // Subspan from index
    std::cout << "From index 2, count 4: ";
    for (int n : span.subspan(2, 4)) std::cout << n << " ";
    std::cout << "\n";
}

// --- 3. Static Extent ---

void staticExtent() {
    std::cout << "\n--- Static Extent ---\n";

    std::array<int, 5> arr = {10, 20, 30, 40, 50};
    std::span<int, 5> span5(arr);  // Compile-time size

    std::cout << "Static span size: " << span5.size() << "\n";

    // Dynamic extent
    std::span<int> dynamic(arr);
    std::cout << "Dynamic span size: " << dynamic.size() << "\n";
}

// --- 4. Function Parameters ---

void processData(std::span<const int> data) {
    std::cout << "Processing " << data.size() << " elements: ";
    for (int n : data) std::cout << n << " ";
    std::cout << "\n";
}

void functionParameters() {
    std::cout << "\n--- Function Parameters ---\n";

    int arr[] = {1, 2, 3};
    processData(arr);

    std::vector<int> vec = {4, 5, 6};
    processData(vec);

    std::array<int, 3> stdarr = {7, 8, 9};
    processData(stdarr);
}

// --- 5. Span of Bytes ---

void spanOfBytes() {
    std::cout << "\n--- Span of Bytes ---\n";

    int value = 42;
    std::span<const char> byteSpan(
        reinterpret_cast<const char*>(&value),
        sizeof(value)
    );

    std::cout << "Bytes of " << value << ": ";
    for (char b : byteSpan) {
        std::cout << static_cast<int>(static_cast<unsigned char>(b)) << " ";
    }
    std::cout << "\n";
}

// --- 6. Span as View ---

void spanAsView() {
    std::cout << "\n--- Span as View ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::span<int> view(vec);

    // Modifying through span modifies original
    for (int& n : view) {
        n *= 10;
    }

    std::cout << "After modification: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 7. Empty Span ---

void emptySpan() {
    std::cout << "\n--- Empty Span ---\n";

    std::span<int> empty;
    std::cout << "Empty span size: " << empty.size() << "\n";
    std::cout << "Empty span is empty: " << empty.empty() << "\n";
}

// --- 8. Span with string ---

void spanWithString() {
    std::cout << "\n--- Span with String ---\n";

    std::string str = "Hello, World!";
    std::span<char> span(str);

    // Find first space
    auto it = std::find(span.begin(), span.end(), ' ');
    if (it != span.end()) {
        std::cout << "First space at index: " << std::distance(span.begin(), it) << "\n";
    }

    // First word
    std::string firstWord(span.begin(), it);
    std::cout << "First word: " << firstWord << "\n";
}

// --- 9. Span of Span ---

void spanOfSpan() {
    std::cout << "\n--- Span of Span ---\n";

    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {4, 5, 6};

    std::span<int> spans[] = {vec1, vec2};

    std::cout << "Span of spans: ";
    for (auto& span : spans) {
        for (int n : span) std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 10. Performance Benefit ---

void performanceBenefit() {
    std::cout << "\n--- Performance Benefit ---\n";

    // Without span: need to pass pointer + size
    auto processRaw = [](const int* data, size_t size) {
        std::cout << "Raw pointer: ";
        for (size_t i = 0; i < size; ++i) std::cout << data[i] << " ";
        std::cout << "\n";
    };

    // With span: cleaner interface
    auto processSpan = [](std::span<const int> data) {
        std::cout << "Span: ";
        for (int n : data) std::cout << n << " ";
        std::cout << "\n";
    };

    std::vector<int> vec = {1, 2, 3, 4, 5};
    processRaw(vec.data(), vec.size());
    processSpan(vec);
}

int main() {
    basicUsage();
    subspan();
    staticExtent();
    functionParameters();
    spanOfBytes();
    spanAsView();
    emptySpan();
    spanWithString();
    spanOfSpan();
    performanceBenefit();

    std::cout << "\n=== All span scenarios completed ===\n";
    return 0;
}
