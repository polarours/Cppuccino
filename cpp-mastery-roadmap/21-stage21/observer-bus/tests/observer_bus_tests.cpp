#include "observer_bus.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_subscribe_publish() {
    observer_bus::EventBus bus;

    std::string received;
    bus.subscribe("event", [&received](const observer_bus::Event& e) {
        received = e.data;
    });

    bus.publish({"event", "test"});
    expect(received == "test", "expected test");
}

void test_multiple_subscribers() {
    observer_bus::EventBus bus;

    int count = 0;
    bus.subscribe("event", [&count](const observer_bus::Event&) { count++; });
    bus.subscribe("event", [&count](const observer_bus::Event&) { count++; });

    bus.publish({"event", ""});
    expect(count == 2, "expected 2 subscribers");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Bus Tests:\n";
        test_subscribe_publish();
        std::cout << "  test_subscribe_publish: PASS\n";

        test_multiple_subscribers();
        std::cout << "  test_multiple_subscribers: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_bus_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_bus_tests passed" << std::endl;
    return 0;
}
