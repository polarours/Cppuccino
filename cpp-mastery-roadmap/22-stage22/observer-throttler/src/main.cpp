#include "observer_throttler.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Observer Throttler Demo ===\n\n";

    observer_throttler::ThrottledDispatcher dispatcher(std::chrono::milliseconds(100));

    int count = 0;
    dispatcher.setHandler([&count](const observer_throttler::Event& e) {
        count++;
        std::cout << "Event: " << e.data << "\n";
    });

    dispatcher.emit({"click", "1"});
    dispatcher.emit({"click", "2"});
    dispatcher.emit({"click", "3"});

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    dispatcher.emit({"click", "4"});

    std::cout << "Total emissions: " << count << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
