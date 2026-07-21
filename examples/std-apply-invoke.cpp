#include <tuple>
#include <iostream>
#include <string>
#include <functional>

// std::apply - invoke a function with arguments from a tuple
// std::invoke - invoke any callable with arguments
// C++17: <tuple>, <functional>

int add(int a, int b) { return a + b; }

struct Multiplier {
    int operator()(int a, int b) const { return a * b; }
};

std::string greet(const std::string& name, int age) {
    return "Hello " + name + " (age " + std::to_string(age) + ")";
}

int main() {
    std::cout << "=== std::apply & std::invoke (C++17) ===\n\n";

    // std::apply: call function with tuple arguments
    auto args = std::make_tuple(3, 4);

    int sum = std::apply(add, args);
    std::cout << "apply(add, {3, 4}) = " << sum << "\n";  // 7

    // std::apply with lambda
    auto result = std::apply([](int a, int b) { return a * b; }, args);
    std::cout << "apply(lambda, {3, 4}) = " << result << "\n";  // 12

    // std::apply with struct
    Multiplier mul;
    result = std::apply(mul, args);
    std::cout << "apply(Multiplier, {3, 4}) = " << result << "\n";  // 12

    // std::invoke: invoke any callable
    std::cout << "\nstd::invoke:\n";

    // Function pointer
    int sum2 = std::invoke(add, 10, 20);
    std::cout << "invoke(add, 10, 20) = " << sum2 << "\n";  // 30

    // Functor
    int prod = std::invoke(Multiplier{}, 5, 6);
    std::cout << "invoke(Multiplier, 5, 6) = " << prod << "\n";  // 30

    // Lambda
    auto square = [](int x) { return x * x; };
    std::cout << "invoke(square, 7) = " << std::invoke(square, 7) << "\n";  // 49

    // Member function via std::invoke
    auto makeGreeting = [](const std::string& name, int age) {
        return greet(name, age);
    };
    std::string greeting = std::apply(makeGreeting, std::make_tuple("Alice", 30));
    std::cout << "\napply(greet, {Alice, 30}) = " << greeting << "\n";

    // Practical use: unpack tuple as function arguments
    auto callWithLog = [](auto&& func, auto&&... args) {
        std::cout << "Calling with " << sizeof...(args) << " args\n";
        return std::invoke(std::forward<decltype(func)>(func),
                           std::forward<decltype(args)>(args)...);
    };

    auto r = callWithLog(add, 100, 200);
    std::cout << "Result: " << r << "\n";  // 300

    return 0;
}
