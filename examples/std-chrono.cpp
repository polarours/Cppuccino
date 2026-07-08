// examples/std-chrono.cpp
// Demonstrates C++17 chrono library for time manipulation.
// Compile with: g++ -std=c++17 -o std_chrono std-chrono.cpp

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <sstream>

// --- 1. Basic types ---

void basicTypes() {
    std::cout << "--- Basic Types ---\n";

    using namespace std::chrono;

    // Duration types
    seconds s(1);
    milliseconds ms(1000);
    microseconds us(1000000);
    nanoseconds ns(1000000000);

    std::cout << "1 second = " << s.count() << " s\n";
    std::cout << "1000 ms = " << ms.count() << " ms\n";
    std::cout << "1000000 us = " << us.count() << " us\n";
    std::cout << "1000000000 ns = " << ns.count() << " ns\n";
}

// --- 2. Duration arithmetic ---

void durationArithmetic() {
    std::cout << "\n--- Duration Arithmetic ---\n";

    using namespace std::chrono;

    seconds a(5);
    seconds b(3);

    std::cout << "5 + 3 = " << (a + b).count() << " s\n";
    std::cout << "5 - 3 = " << (a - b).count() << " s\n";
    std::cout << "5 * 2 = " << (a * 2).count() << " s\n";
    std::cout << "10 / 2 = " << (seconds(10) / 2).count() << " s\n";
}

// --- 3. Duration conversion ---

void durationConversion() {
    std::cout << "\n--- Duration Conversion ---\n";

    using namespace std::chrono;

    milliseconds ms(1500);

    std::cout << "1500 ms = " << duration_cast<seconds>(ms).count() << " s\n";
    std::cout << "1500 ms = " << duration_cast<minutes>(ms).count() << " min\n";
    std::cout << "1 s = " << duration_cast<milliseconds>(seconds(1)).count() << " ms\n";
}

// --- 4. Time point ---

void timePoint() {
    std::cout << "\n--- Time Point ---\n";

    using namespace std::chrono;

    auto now = system_clock::now();
    auto duration = now.time_since_epoch();

    std::cout << "Since epoch: " << duration_cast<seconds>(duration).count() << " s\n";
    std::cout << "Since epoch: " << duration_cast<milliseconds>(duration).count() << " ms\n";
}

// --- 5. System clock ---

void systemClock() {
    std::cout << "\n--- System Clock ---\n";

    using namespace std::chrono;

    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);

    std::cout << "Current time: " << std::ctime(&time);

    // High resolution clock
    auto highRes = high_resolution_clock::now();
    std::cout << "High resolution: " << highRes.time_since_epoch().count() << " ticks\n";
}

// --- 6. Steady clock ---

void steadyClock() {
    std::cout << "\n--- Steady Clock ---\n";

    using namespace std::chrono;

    auto start = steady_clock::now();

    // Simulate work
    std::this_thread::sleep_for(milliseconds(10));

    auto end = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "Elapsed: " << duration.count() << " us\n";
}

// --- 7. Duration literals ---

void durationLiterals() {
    std::cout << "\n--- Duration Literals ---\n";

    using namespace std::chrono_literals;

    auto sec = 1s;
    auto ms = 100ms;
    auto us = 1000us;
    auto ns = 1000ns;

    std::cout << "1s = " << sec.count() << " s\n";
    std::cout << "100ms = " << ms.count() << " ms\n";
    std::cout << "1000us = " << us.count() << " us\n";
    std::cout << "1000ns = " << ns.count() << " ns\n";
}

// --- 8. Time formatting ---

void timeFormatting() {
    std::cout << "\n--- Time Formatting ---\n";

    using namespace std::chrono;

    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);

    struct tm* tm = std::localtime(&time);

    std::cout << "Year: " << tm->tm_year + 1900 << "\n";
    std::cout << "Month: " << tm->tm_mon + 1 << "\n";
    std::cout << "Day: " << tm->tm_mday << "\n";
    std::cout << "Hour: " << tm->tm_hour << "\n";
    std::cout << "Minute: " << tm->tm_min << "\n";
    std::cout << "Second: " << tm->tm_sec << "\n";
}

// --- 9. Timer example ---

void timerExample() {
    std::cout << "\n--- Timer Example ---\n";

    using namespace std::chrono;

    auto start = steady_clock::now();

    // Do some work
    volatile int sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i;
    }

    auto end = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "Computed sum = " << sum << "\n";
    std::cout << "Time: " << duration.count() << " us\n";
}

// --- 10. Date and time ---

void dateTime() {
    std::cout << "\n--- Date and Time ---\n";

    using namespace std::chrono;

    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);

    std::cout << "Date: " << std::put_time(std::localtime(&time), "%Y-%m-%d") << "\n";
    std::cout << "Time: " << std::put_time(std::localtime(&time), "%H:%M:%S") << "\n";
    std::cout << "DateTime: " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\n";
}

int main() {
    basicTypes();
    durationArithmetic();
    durationConversion();
    timePoint();
    systemClock();
    steadyClock();
    durationLiterals();
    timeFormatting();
    timerExample();
    dateTime();

    std::cout << "\n=== All chrono scenarios completed ===\n";
    return 0;
}
