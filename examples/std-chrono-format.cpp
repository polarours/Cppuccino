#include <iostream>
#include <chrono>
#include <format>
#include <string>

// std::chrono formatting - C++20
// Format chrono types with std::format

int main() {
    std::cout << "=== std::chrono Format Demo (C++20) ===\n\n";

    // Duration formatting
    std::cout << "1. Duration formatting:\n";
    auto seconds = std::chrono::seconds(45);
    auto milliseconds = std::chrono::milliseconds(500);
    auto hours = std::chrono::hours(2);

    std::cout << "  Seconds: " << std::format("{}", seconds.count()) << " s\n";
    std::cout << "  Milliseconds: " << std::format("{}", milliseconds.count()) << " ms\n";
    std::cout << "  Hours: " << std::format("{}", hours.count()) << " h\n";

    // Duration arithmetic
    std::cout << "\n2. Duration arithmetic:\n";
    auto total = hours + seconds + milliseconds;
    auto totalSec = std::chrono::duration_cast<std::chrono::seconds>(total);
    std::cout << "  Total seconds: " << totalSec.count() << "\n";

    // Time point with strftime
    std::cout << "\n3. Time point:\n";
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&now_c);
    char timeStr[64];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    std::cout << "  Current time: " << timeStr << "\n";

    // Duration with custom format
    std::cout << "\n4. Custom duration format:\n";
    auto duration = std::chrono::minutes(90) + std::chrono::seconds(30);
    auto mins = std::chrono::duration_cast<std::chrono::minutes>(duration);
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(duration) % 60;
    std::cout << "  Duration: " << mins.count() << " minutes " << secs.count() << " seconds\n";

    return 0;
}
