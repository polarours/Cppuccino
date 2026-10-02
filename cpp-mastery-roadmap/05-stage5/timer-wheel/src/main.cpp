#include "timer_wheel.hpp"

#include <iostream>

int main() {
    timer_wheel::TimerWheel wheel(/*slots=*/16, /*tickIntervalMs=*/10);
    int fired = 0;

    wheel.scheduleIn(2, [&] { std::cout << "t+2: fired\n"; ++fired; });
    auto keep = wheel.scheduleIn(5, [&] { std::cout << "t+5: fired\n"; ++fired; });
    wheel.scheduleIn(5, [&] { std::cout << "t+5b: fired\n"; ++fired; });

    wheel.cancel(keep);  // cancel the first t+5 timer

    for (int i = 0; i < 6; ++i) wheel.tick();

    std::cout << "fired=" << fired << " (expect 2: t+2 and t+5b)\n";
    std::cout << "current tick: " << wheel.currentTick() << "\n";
    return 0;
}
