#include "observer_buffer.hpp"

#include <iostream>

int main() {
    std::cout << "=== Observer Buffer Demo ===\n\n";

    observer_buffer::BufferedDispatcher dispatcher(3);

    int count = 0;
    dispatcher.setHandler([&count](const observer_buffer::Event& e) {
        count++;
        std::cout << "Event: " << e.data << "\n";
    });

    dispatcher.emit({"event", "1"});
    dispatcher.emit({"event", "2"});
    dispatcher.emit({"event", "3"});

    std::cout << "Flushed " << count << " events\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
