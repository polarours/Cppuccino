// examples/lambda-expressions.cpp
// Demonstrates lambda expressions in C++11/14/17.
// Compile with: g++ -std=c++17 -o lambda_expressions lambda-expressions.cpp

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>

// --- 1. Basic Lambda ---

void basicLambda() {
    std::cout << "--- Basic Lambda ---\n";

    auto greet = []() {
        std::cout << "Hello, Lambda!\n";
    };
    greet();

    auto add = [](int a, int b) -> int {
        return a + b;
    };
    std::cout << "add(3, 4) = " << add(3, 4) << "\n";
}

// --- 2. Capture Modes ---

void captureModes() {
    std::cout << "\n--- Capture Modes ---\n";

    int x = 10;
    int y = 20;

    // Capture by value
    auto byValue = [x, y]() {
        std::cout << "byValue: x=" << x << ", y=" << y << "\n";
    };

    // Capture by reference
    auto byRef = [&x, &y]() {
        std::cout << "byRef: x=" << x << ", y=" << y << "\n";
    };

    // Capture all by value
    auto allByValue = [=]() {
        std::cout << "allByValue: x=" << x << ", y=" << y << "\n";
    };

    // Capture all by reference
    auto allByRef = [&]() {
        std::cout << "allByRef: x=" << x << ", y=" << y << "\n";
    };

    byValue();
    byRef();
    allByValue();
    allByRef();

    // Modify through reference
    allByRef();
    x = 100;
    std::cout << "After modification: x=" << x << "\n";
}

// --- 3. Mutable Lambda ---

void mutableLambda() {
    std::cout << "\n--- Mutable Lambda ---\n";

    int counter = 0;

    // Non-mutable: cannot modify captured value
    // auto nonMutable = [counter]() { counter++; };  // ERROR

    // Mutable: can modify captured copy
    auto mutableLambda = [counter]() mutable {
        counter++;
        std::cout << "counter = " << counter << "\n";
    };

    mutableLambda();
    mutableLambda();
    mutableLambda();

    std::cout << "Original counter = " << counter << "\n";
}

// --- 4. Lambda with STL ---

void lambdaWithSTL() {
    std::cout << "\n--- Lambda with STL ---\n";

    std::vector<int> nums = {5, 2, 8, 1, 9, 3};

    // std::sort with lambda
    std::sort(nums.begin(), nums.end(), [](int a, int b) {
        return a > b;  // Descending
    });

    std::cout << "Sorted descending: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";

    // std::for_each
    std::cout << "Squares: ";
    std::for_each(nums.begin(), nums.end(), [](int n) {
        std::cout << n * n << " ";
    });
    std::cout << "\n";

    // std::find_if
    auto it = std::find_if(nums.begin(), nums.end(), [](int n) {
        return n > 7;
    });
    if (it != nums.end()) {
        std::cout << "First > 7: " << *it << "\n";
    }

    // std::accumulate with lambda
    int sum = 0;
    std::for_each(nums.begin(), nums.end(), [&sum](int n) {
        sum += n;
    });
    std::cout << "Sum: " << sum << "\n";
}

// --- 5. Generic Lambda (C++14) ---

void genericLambda() {
    std::cout << "\n--- Generic Lambda (C++14) ---\n";

    auto print = [](const auto& x) {
        std::cout << x << "\n";
    };

    print(42);
    print(3.14);
    print("Hello");
    print(std::string("World"));

    auto add = [](auto a, auto b) {
        return a + b;
    };

    std::cout << "add(3, 4) = " << add(3, 4) << "\n";
    std::cout << "add(1.5, 2.5) = " << add(1.5, 2.5) << "\n";
}

// --- 6. Lambda as Function Pointer ---

void lambdaAsFunctionPointer() {
    std::cout << "\n--- Lambda as Function Pointer ---\n";

    // Stateless lambda can convert to function pointer
    int (*funcPtr)(int, int) = [](int a, int b) {
        return a + b;
    };

    std::cout << "funcPtr(3, 4) = " << funcPtr(3, 4) << "\n";

    // Using function pointer
    auto apply = [](int (*f)(int, int), int a, int b) {
        return f(a, b);
    };

    std::cout << "apply(funcPtr, 5, 6) = " << apply(funcPtr, 5, 6) << "\n";
}

// --- 7. std::function ---

void stdFunction() {
    std::cout << "\n--- std::function ---\n";

    std::function<int(int, int)> func;

    func = [](int a, int b) { return a + b; };
    std::cout << "add: " << func(3, 4) << "\n";

    func = [](int a, int b) { return a * b; };
    std::cout << "multiply: " << func(3, 4) << "\n";

    // std::function can hold stateful lambdas
    int factor = 10;
    func = [factor](int a, int b) { return (a + b) * factor; };
    std::cout << "scaled sum: " << func(3, 4) << "\n";
}

// --- 8. Lambda with Smart Pointers ---

void lambdaWithSmartPointers() {
    std::cout << "\n--- Lambda with Smart Pointers ---\n";

    auto ptr = std::make_unique<int>(42);

    // Capture by move
    auto func = [p = std::move(ptr)]() {
        std::cout << "Value: " << *p << "\n";
    };

    func();
    // ptr is now empty
}

// --- 9. Immediate Lambda (C++20 preview) ---

void immediateLambda() {
    std::cout << "\n--- IIFE Pattern ---\n";

    // Immediately Invoked Function Expression
    int result = [](int x) {
        return x * x;
    }(5);

    std::cout << "IIFE result: " << result << "\n";
}

// --- 10. Recursive Lambda ---

void recursiveLambda() {
    std::cout << "\n--- Recursive Lambda ---\n";

    // Using std::function for recursion
    std::function<int(int)> factorial = [&factorial](int n) {
        return (n <= 1) ? 1 : n * factorial(n - 1);
    };

    std::cout << "5! = " << factorial(5) << "\n";

    // Using Y-combinator pattern
    auto ycomb = [](auto self, int n) -> int {
        return (n <= 1) ? 1 : n * self(self, n - 1);
    };

    std::cout << "6! = " << ycomb(ycomb, 6) << "\n";
}

int main() {
    basicLambda();
    captureModes();
    mutableLambda();
    lambdaWithSTL();
    genericLambda();
    lambdaAsFunctionPointer();
    stdFunction();
    lambdaWithSmartPointers();
    immediateLambda();
    recursiveLambda();

    std::cout << "\n=== All lambda scenarios completed ===\n";
    return 0;
}
