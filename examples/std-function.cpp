// examples/std-function.cpp
// Demonstrates std::function for function objects and callbacks.
// Compile with: g++ -std=c++17 -o std_function std-function.cpp

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

// --- 1. Basic usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    std::function<int(int, int)> add = [](int a, int b) { return a + b; };
    std::cout << "add(3, 4) = " << add(3, 4) << "\n";

    std::function<void()> greet = []() { std::cout << "Hello!\n"; };
    greet();
}

// --- 2. Different callable types ---

int freeFunction(int x) { return x * 2; }

struct Functor {
    int operator()(int x) { return x * 3; }
};

void callableTypes() {
    std::cout << "\n--- Different Callable Types ---\n";

    // Lambda
    std::function<int(int)> f1 = [](int x) { return x + 1; };
    std::cout << "Lambda: " << f1(5) << "\n";

    // Free function
    std::function<int(int)> f2 = freeFunction;
    std::cout << "Free function: " << f2(5) << "\n";

    // Functor
    std::function<int(int)> f3 = Functor{};
    std::cout << "Functor: " << f3(5) << "\n";
}

// --- 3. Stateful lambdas ---

void statefulLambdas() {
    std::cout << "\n--- Stateful Lambdas ---\n";

    int counter = 0;
    std::function<int()> increment = [&counter]() { return ++counter; };

    std::cout << "1: " << increment() << "\n";
    std::cout << "2: " << increment() << "\n";
    std::cout << "3: " << increment() << "\n";
}

// --- 4. Callbacks ---

void processItems(const std::vector<int>& items, std::function<void(int)> callback) {
    for (const auto& item : items) {
        callback(item);
    }
}

void callbacksDemo() {
    std::cout << "\n--- Callbacks ---\n";

    std::vector<int> items = {1, 2, 3, 4, 5};

    std::cout << "Print: ";
    processItems(items, [](int x) { std::cout << x << " "; });
    std::cout << "\n";

    std::cout << "Double: ";
    processItems(items, [](int x) { std::cout << x * 2 << " "; });
    std::cout << "\n";
}

// --- 5. Function as class member ---

class Processor {
public:
    using Callback = std::function<void(const std::string&)>;

    void setCallback(Callback cb) { callback_ = cb; }

    void process(const std::string& data) {
        if (callback_) {
            callback_(data);
        }
    }

private:
    Callback callback_;
};

void memberFunction() {
    std::cout << "\n--- Function as Class Member ---\n";

    Processor processor;
    processor.setCallback([](const std::string& data) {
        std::cout << "Processed: " << data << "\n";
    });

    processor.process("test data");
}

// --- 6. Higher-order functions ---

std::function<int(int)> compose(std::function<int(int)> f, std::function<int(int)> g) {
    return [f, g](int x) { return f(g(x)); };
}

void higherOrderFunctions() {
    std::cout << "\n--- Higher-Order Functions ---\n";

    auto doubleIt = [](int x) { return x * 2; };
    auto addTen = [](int x) { return x + 10; };

    auto composed = compose(doubleIt, addTen);
    std::cout << "compose(double, add10)(5) = " << composed(5) << "\n";
}

// --- 7. Function with different return types ---

void differentReturnTypes() {
    std::cout << "\n--- Different Return Types ---\n";

    std::function<bool(int)> isPositive = [](int x) { return x > 0; };
    std::function<std::string(int)> toString = [](int x) { return std::to_string(x); };

    std::cout << "isPositive(5): " << std::boolalpha << isPositive(5) << "\n";
    std::cout << "toString(42): " << toString(42) << "\n";
}

// --- 8. Function in algorithm ---

void functionInAlgorithm() {
    std::cout << "\n--- Function in Algorithm ---\n";

    std::vector<int> nums = {5, 3, 1, 4, 2};

    std::sort(nums.begin(), nums.end(), std::function<bool(int, int)>([](int a, int b) {
        return a > b;
    }));

    std::cout << "Sorted descending: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";
}

// --- 9. Practical example ---

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    class EventSystem {
    public:
        using Handler = std::function<void(const std::string&)>;

        void subscribe(const std::string& event, Handler handler) {
            handlers_[event].push_back(handler);
        }

        void emit(const std::string& event) {
            for (auto& handler : handlers_[event]) {
                handler(event);
            }
        }

    private:
        std::map<std::string, std::vector<Handler>> handlers_;
    };

    EventSystem events;
    events.subscribe("click", [](const std::string& e) { std::cout << "Click handler: " << e << "\n"; });
    events.subscribe("click", [](const std::string& e) { std::cout << "Analytics: " << e << "\n"; });

    events.emit("click");
}

int main() {
    basicUsage();
    callableTypes();
    statefulLambdas();
    callbacksDemo();
    memberFunction();
    higherOrderFunctions();
    differentReturnTypes();
    functionInAlgorithm();
    practicalExample();

    std::cout << "\n=== All function scenarios completed ===\n";
    return 0;
}
