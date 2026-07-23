#include <chrono>
#include <iostream>
#include <thread>

// std::chrono - time library (C++11, enhanced in C++17/20)
// <chrono>

int main() {
    std::cout << "=== std::chrono ===\n\n";

    // Clock types
    auto now = std::chrono::system_clock::now();
    auto steady = std::chrono::steady_clock::now();
    auto highRes = std::chrono::high_resolution_clock::now();

    std::cout << "System clock time: " << now.time_since_epoch().count() << "\n";
    std::cout << "Steady clock time: " << steady.time_since_epoch().count() << "\n";

    // Duration
    auto ms = std::chrono::milliseconds(100);
    auto sec = std::chrono::seconds(5);
    auto min = std::chrono::minutes(2);

    std::cout << "\nDuration conversions:\n";
    std::cout << "  100ms = " << std::chrono::duration_cast<std::chrono::milliseconds>(ms).count() << " ms\n";
    std::cout << "  5s = " << std::chrono::duration_cast<std::chrono::seconds>(sec).count() << " s\n";
    std::cout << "  2min = " << std::chrono::duration_cast<std::chrono::minutes>(min).count() << " min\n";

    // Timing measurement
    auto start = std::chrono::high_resolution_clock::now();
    // Do some work
    volatile int sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "\nLoop time: " << duration.count() << " microseconds\n";

    // Time point arithmetic
    auto later = now + std::chrono::hours(24);
    auto diff = later - now;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(diff).count();
    std::cout << "\n24 hours later: " << hours << " hours\n";

    // Steady clock for measuring intervals
    auto t1 = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t2 = std::chrono::steady_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "\nSleep measurement: " << elapsed.count() << " ms\n";

    // C++17: std::chrono literals
    using namespace std::chrono_literals;
    auto twoSec = 2s;
    auto halfSec = 500ms;
    std::cout << "\nLiterals: " << twoSec.count() << "s, "
              << halfSec.count() << "ms\n";

    return 0;
}
