#include <iostream>
#include <chrono>
#include <format>
#include <string>

// C++20 chrono literals and formatting
// Demonstrates time duration literals and chrono formatting

namespace chrono = std::chrono;

int main() {
    std::cout << "=== C++20 Chrono Literals Demo ===\n\n";

    // Chrono literals (C++14)
    std::cout << "1. Chrono literals:\n";
    using namespace std::chrono_literals;
    
    auto one_sec = 1s;
    auto half_min = 30s;
    auto two_hours = 2h;
    auto thirty_min = 30min;
    
    std::cout << "   1 second = " << one_sec.count() << " seconds\n";
    std::cout << "   30 seconds = " << thirty_min.count() << " seconds\n";
    std::cout << "   2 hours = " << two_hours.count() << " hours\n";

    // Duration arithmetic
    std::cout << "\n2. Duration arithmetic:\n";
    auto meeting = 30min + 45s;  // 30 minutes and 45 seconds
    std::cout << "   Meeting duration: " << meeting.count() << " seconds\n";
    std::cout << "   Meeting duration: " << std::format("{}", std::chrono::duration_cast<std::chrono::minutes>(meeting)) << " minutes\n";

    // Time point operations
    std::cout << "\n3. Time points:\n";
    auto now = chrono::system_clock::now();
    auto tomorrow = now + 24h;
    
    auto duration = tomorrow - now;
    std::cout << "   Time until tomorrow: " << duration.count() << " seconds\n";

    // chrono formatting (C++20)
    std::cout << "\n4. Chrono formatting:\n";
    auto duration_min = 90min + 30s;
    std::cout << "   90 minutes 30 seconds = " 
              << std::format("{:%M:%S}", duration_min) << "\n";

    // duration_cast
    std::cout << "\n5. Duration conversions:\n";
    auto millis = 1500ms;
    std::cout << "   1500ms = " 
              << std::format("{:.1f} seconds", 
                  std::chrono::duration<double, std::ratio<1>>(millis).count()) 
              << "\n";

    // std::chrono::zoned_time (C++20) - timezone aware time
    std::cout << "\n6. Zoned time (if supported):\n";
    try {
        auto tz = chrono::locate_zone("UTC");
        auto zt = chrono::zoned_time(tz, now);
        std::cout << "   Current time in UTC: " << zt << "\n";
    } catch (...) {
        std::cout << "   (timezone support not available)\n";
    }

    // Duration to string formatting
    std::cout << "\n7. Duration string formatting:\n";
    auto hours_min_sec = 3h + 45min + 30s;
    auto h = std::chrono::duration_cast<std::chrono::hours>(hours_min_sec).count();
    auto m = std::chrono::duration_cast<std::chrono::minutes>(hours_min_sec % 1h).count();
    auto s = std::chrono::duration_cast<std::chrono::seconds>(hours_min_sec % 1min).count();
    std::cout << "   3h 45m 30s formatted: " << h << ":" << m << ":" << s << "\n";

    return 0;
}
