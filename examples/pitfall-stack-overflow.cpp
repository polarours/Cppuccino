// examples/pitfall-stack-overflow.cpp
// Demonstrates stack overflow from unbounded recursion and the iterative fix.
// Compile with: g++ -std=c++17 -o pitfall_stack_overflow pitfall-stack-overflow.cpp

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

// --- 1. Unbounded recursion ---

// This is what a stack overflow looks like. Every call pushes a new frame,
// so the process is killed with SIGSEGV once the stack is exhausted.
// void countdown() {
//     countdown();   // no base case -> the stack runs out
// }

// --- 2. How much stack does one frame cost? ---

constexpr int kProbeDepth = 16;
std::uintptr_t frameAddresses[kProbeDepth + 1] = {};
volatile std::uintptr_t deepestResult = 0;

// Recurses `depth` levels deep and records where each frame lives.
std::uintptr_t probeFrames(int depth) {
    char padding[64] = {};  // a known-size local: every level needs its own
    padding[0] = static_cast<char>(depth);
    const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(padding);
    frameAddresses[depth] = address;
    if (depth == 0) {
        return address;
    }
    const std::uintptr_t inner = probeFrames(depth - 1);
    // The result is stored in a global, so the call cannot be turned into a
    // tail call: every level really does keep its own stack frame.
    deepestResult = inner;
    return inner;
}

void measureStackFrame() {
    std::cout << "--- 2. How much stack does one frame cost? ---\n";
    probeFrames(kProbeDepth);
    const std::uintptr_t delta = frameAddresses[kProbeDepth] - frameAddresses[0];
    const std::uintptr_t perLevel = delta / static_cast<std::uintptr_t>(kProbeDepth);
    std::cout << "  " << kProbeDepth << " levels of recursion moved the stack by " << delta
              << " bytes\n";
    std::cout << "  about " << perLevel << " bytes per level for this function\n";
    std::cout << "  deepest frame address: " << deepestResult << "\n";
    std::cout << "  so a recursion 1000000 levels deep would need roughly "
              << (perLevel * 1000000) / 1024 / 1024 << " MiB of stack -- far more than the usual 8 MiB\n";
}

// --- 3. Deep but "legitimate looking" recursion ---

long long recursiveSum(const std::vector<long long>& values, std::size_t index) {
    if (index == values.size()) {
        return 0;
    }
    return values[index] + recursiveSum(values, index + 1);  // depth == values.size()
}

long long boundedRecursiveSum(const std::vector<long long>& values, std::size_t index,
                              std::size_t limit) {
    if (index == values.size()) {
        return 0;
    }
    if (index >= limit) {
        throw std::runtime_error("recursion depth limit exceeded");
    }
    return values[index] + boundedRecursiveSum(values, index + 1, limit);
}

long long iterativeSum(const std::vector<long long>& values) {
    long long total = 0;  // the state lives in a loop, not on the stack
    for (long long value : values) {
        total += value;
    }
    return total;
}

void deepRecursion() {
    std::cout << "\n--- 3. Deep linear recursion ---\n";

    std::vector<long long> values(10000, 1);
    std::cout << "  recursive sum of " << values.size() << " elements: "
              << recursiveSum(values, 0) << "\n";
    std::cout << "  iterative  sum of " << values.size() << " elements: "
              << iterativeSum(values) << "\n";

    // The same recursive function on ten million elements would need ten
    // million frames and would blow the stack, so a depth limit is checked
    // instead of letting the process crash.
    try {
        std::vector<long long> huge(100000, 1);
        (void)boundedRecursiveSum(huge, 0, 1000);
    } catch (const std::runtime_error& e) {
        std::cout << "  bounded recursion refused to go deeper: " << e.what() << "\n";
    }
}

// --- 4. Big local objects belong on the heap ---

void largeLocalObject() {
    std::cout << "\n--- 4. Big local objects belong on the heap ---\n";

    // BAD: a 1 MiB array is a single stack frame, and a few of them in a
    // call chain overflow the stack.
    // char buffer[1 << 20] = {};

    // GOOD: a std::vector allocates its storage on the heap.
    std::vector<char> buffer(1 << 20, 'x');
    std::cout << "  heap buffer size: " << buffer.size() << " bytes, first byte = '"
              << buffer.front() << "'\n";
}

int main() {
    measureStackFrame();
    deepRecursion();
    largeLocalObject();

    std::cout << "\n=== All stack overflow scenarios completed ===\n";
    return 0;
}
