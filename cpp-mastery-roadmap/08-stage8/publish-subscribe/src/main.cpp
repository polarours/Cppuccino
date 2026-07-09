#include "pubsub.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== Publish-Subscribe Demo ===\n\n";

    pubsub::Publisher<std::string> pub;

    // Subscribe to topics
    auto id1 = pub.subscribe("news", [](const std::string& msg) {
        std::cout << "[News] " << msg << "\n";
    });

    auto id2 = pub.subscribe("sports", [](const std::string& msg) {
        std::cout << "[Sports] " << msg << "\n";
    });

    auto id3 = pub.subscribe("news", [](const std::string& msg) {
        std::cout << "[Analytics] News: " << msg << "\n";
    });

    // Publish messages
    pub.publish("news", "Breaking: New C++ standard released!");
    pub.publish("sports", "Team wins championship!");

    std::cout << "\nNews subscribers: " << pub.subscriberCount("news") << "\n";
    std::cout << "Sports subscribers: " << pub.subscriberCount("sports") << "\n";

    // Unsubscribe
    pub.unsubscribe(id3);
    std::cout << "\nAfter unsubscribe:\n";
    std::cout << "News subscribers: " << pub.subscriberCount("news") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
