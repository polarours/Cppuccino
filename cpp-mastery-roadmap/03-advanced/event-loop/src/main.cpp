#include "event_loop.hpp"

#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Event Loop Demo ===\n\n";

    event_loop::EventLoop loop;

    std::cout << "Posting immediate tasks...\n";
    for (int i = 0; i < 3; ++i) {
        loop.post([i]() {
            std::cout << "  Task " << i << " executed\n";
        });
    }

    std::cout << "\nScheduling delayed task (100ms)...\n";
    loop.schedule([]() {
        std::cout << "  Delayed task executed\n";
    }, std::chrono::milliseconds(100));

    std::cout << "\nScheduling repeating task (50ms interval)...\n";
    int count = 0;
    event_loop::TimerId repeatId = 0;
    repeatId = loop.scheduleRepeating([&]() {
        count++;
        std::cout << "  Repeat " << count << "\n";
        if (count >= 3) {
            loop.cancelTimer(repeatId);
        }
    }, std::chrono::milliseconds(50));

    std::cout << "\nRunning event loop...\n";
    loop.schedule([&loop]() {
        std::cout << "\nStopping event loop...\n";
        loop.stop();
    }, std::chrono::milliseconds(500));

    loop.run();

    std::cout << "\nEvent loop stopped\n";
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
