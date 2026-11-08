#include "guarded_suspension.hpp"

#include <iostream>
#include <thread>

int main() {
    std::cout << "=== Guarded Suspension Demo ===\n\n";

    guarded_suspension::GuardedObject obj;

    std::thread producer([&obj]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        obj.set(42);
    });

    // The predicate asks the object about itself. That is the natural
    // thing to write, and it works: the state of the object is atomic, so
    // the predicate does not wait for the lock the waiting thread holds.
    bool result = obj.waitFor([&obj]() { return obj.isReady(); });
    std::cout << "Wait result: " << std::boolalpha << result << "\n";
    std::cout << "Value: " << obj.get() << "\n";

    producer.join();
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
