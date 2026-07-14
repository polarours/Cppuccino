#include "observer_subscription.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Observer Subscription Demo ===\n\n";

    observer_subscription::EventBus bus;

    auto sub = bus.subscribe("message", [](const observer_subscription::Event& e) {
        std::cout << "Received: " << e.data << "\n";
    });

    bus.publish({"message", "Hello!"});
    bus.publish({"message", "World!"});

    sub->unsubscribe();
    bus.publish({"message", "Nobody hears this"});

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
