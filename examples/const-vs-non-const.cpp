// examples/const-vs-non-const.cpp
// Demonstrates the performance difference between const and non-const methods.
// Compile with: g++ -std=c++17 -o const_vs_non_const const-vs-non-const.cpp


#include <iostream>
#include <chrono>

class Data {
public:
    // The sum of a million numbers does not fit in an int: 0 + 1 + ... +
    // 999999 is 499999500000, which overflows a 32 bit signed integer.
    // An overflow there is undefined behaviour, not a wrap-around.
    long long compute() const {
        long long sum = 0;
        for (int i = 0; i < 1000000; ++i) {
            sum += i;
        }
        return sum;
    }

    long long computeNonConst() {
        long long sum = 0;
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
