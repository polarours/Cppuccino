// examples/chrono-calendar.cpp
// Demonstrates the C++20 chrono calendar: year/month/day arithmetic,
// weekday queries, and date differences (no third-party date library).
// Compile: g++ -std=c++20 -Wall -Wextra -o chrono-calendar chrono-calendar.cpp

#include <chrono>
#include <iostream>

namespace calendar_demo {

using namespace std::chrono;

// A date is just a year_month_day; convert to sys_days for day arithmetic
int weekdayIndex(const year_month_day& d) {
    return static_cast<int>(weekday{sys_days{d}}.c_encoding());  // 0=Sunday
}

std::string weekdayName(const year_month_day& d) {
    static const char* names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    return names[weekdayIndex(d)];
}

year_month_day addDays(const year_month_day& d, int n) {
    return year_month_day{sys_days{d} + days{n}};
}

year_month_day endOfMonth(const year_month_day& d) {
    return year_month_day{d.year() / d.month() / std::chrono::last};
}

bool isLeap(const year& y) {
    return y.is_leap();
}

} // namespace calendar_demo

int main() {
    using namespace calendar_demo;
    std::cout << "=== C++20 chrono Calendar Demo ===\n\n";

    year_month_day today{2026y, October, 1d};
    std::cout << "1. Construction and validation:\n";
    std::cout << "  today = " << today << "  ok=" << std::boolalpha << today.ok() << "\n";
    year_month_day bad{2026y, February, 30d};
    std::cout << "  2026-02-30 ok=" << bad.ok() << "  (invalid date detected)\n";

    std::cout << "\n2. Weekday queries:\n";
    std::cout << "  " << today << " is " << weekdayName(today)
              << " (index " << weekdayIndex(today) << ")\n";
    year_month_day xmas{2026y, December, 25d};
    std::cout << "  " << xmas << " is " << weekdayName(xmas) << "\n";

    std::cout << "\n3. Date arithmetic:\n";
    std::cout << "  +30 days from " << today << " -> " << addDays(today, 30) << "\n";
    std::cout << "  end of month    -> " << endOfMonth(today) << "\n";

    std::cout << "\n4. Month/year arithmetic:\n";
    year_month nextMonth = today.year() / today.month() + months{1};
    year_month_day nm{nextMonth / today.day()};
    std::cout << "  same day next month: " << nm << "\n";
    year_month_day jan1{2027y, January, 1d};
    std::cout << "  days from today to 2027-01-01: "
              << (sys_days{jan1} - sys_days{today}).count() << "\n";

    std::cout << "\n5. Leap years:\n";
    std::cout << "  2024 leap: " << isLeap(2024y) << "\n";
    std::cout << "  2026 leap: " << isLeap(2026y) << "\n";
    std::cout << "  2100 leap: " << isLeap(2100y) << "  (divisible by 100)\n";
    std::cout << "  2000 leap: " << isLeap(2000y) << "  (divisible by 400)\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
