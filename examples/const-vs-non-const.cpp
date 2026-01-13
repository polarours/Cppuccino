// examples/const_vs_nonconst.cpp
// This example demonstrates the performance difference between const and non-const methods in C++.

#include <iostream>
#include <chrono>

class Data {
public:
    int compute() const {
        int sum = 0;
        for (int i = 0; i < 1000000; ++i) {
            sum += i;
        }
        return sum;
    }

    int computeNonConst() {
        int sum = 0;
        for (int i = 0; i < 1000000; ++i) {
            sum += i;
        }
        return sum;
    }
};

// g++ -std=c++17 -o const_vs_nonconst const_vs_nonconst.cpp && ./const_vs_nonconst
int main() {
    Data data;

    auto startConst = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        data.compute(); 
    }
    auto endConst = std::chrono::high_resolution_clock::now();

    auto startNonConst = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        data.computeNonConst(); 
    }
    auto endNonConst = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> constDuration = endConst - startConst;
    std::chrono::duration<double, std::milli> nonConstDuration = endNonConst - startNonConst;
    std::cout << "Const method duration: " << constDuration.count() << " ms\n";
    std::cout << "Non-const method duration: " << nonConstDuration.count() << " ms\n";
}
