#include "observer_debouncer.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Observer Debouncer Demo ===\n\n";

    observer_debouncer::DebouncedDispatcher dispatcher(std::chrono::milliseconds(100));

    dispatcher.setHandler([](const observer_debouncer::Event& e) {
        std::cout << "Handler fired: " << e.data << "\n";
    });

    dispatcher.emit({"click", "fast1"});
    dispatcher.emit({"click", "fast2"});
    dispatcher.emit({"click", "fast3"});

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Total emits: " << dispatcher.emitCount() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
