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

// The bus knows how many handlers an event has.
void test_handler_count() {
    observer_bus::EventBus bus;

    expect(bus.handlerCount("click") == 0, "expected no handlers");

    bus.subscribe("click", [](const observer_bus::Event&) {});
    expect(bus.handlerCount("click") == 1, "expected 1 handler");

    bus.subscribe("click", [](const observer_bus::Event&) {});
    expect(bus.handlerCount("click") == 2, "expected 2 handlers");

    // The handlers of one event are not the handlers of another.
    expect(bus.handlerCount("scroll") == 0, "expected no handlers for scroll");
}

// A handler that subscribes during a dispatch must not invalidate the
// loop. The new handler starts with the next publish, not this one.
void test_subscribe_during_publish() {
    observer_bus::EventBus bus;

    int calls = 0;
    int extra = 0;
    bus.subscribe("e", [&calls](const observer_bus::Event&) { ++calls; });
    bus.subscribe("e", [&calls, &bus, &extra](const observer_bus::Event&) {
        ++calls;
        bus.subscribe("e", [&extra](const observer_bus::Event&) { ++extra; });
    });

    bus.publish({"e", "x"});
    expect(calls == 2, "only the handlers present at dispatch time run now");
    expect(extra == 0, "the new handler does not join this dispatch");
    expect(bus.handlerCount("e") == 3, "the new handler is registered");

    bus.publish({"e", "y"});
    expect(calls == 4, "the original two handlers run again");
    expect(extra == 1, "the new handler runs on the next publish");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Bus Tests:\n";
        test_subscribe_publish();
        std::cout << "  test_subscribe_publish: PASS\n";

        test_multiple_subscribers();
        std::cout << "  test_multiple_subscribers: PASS\n";

        test_handler_count();
        std::cout << "  test_handler_count: PASS\n";

        test_subscribe_during_publish();
        std::cout << "  test_subscribe_during_publish: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "observer_bus_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nobserver_bus_tests passed" << std::endl;
    return 0;
}
