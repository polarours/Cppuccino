#include "observer_subscription.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_subscribe_and_publish() {
    observer_subscription::EventBus bus;

    std::string received;
    auto sub = bus.subscribe("click", [&received](const observer_subscription::Event& e) {
        received = e.data;
    });

    bus.publish({"click", "hello"});
    expect(received == "hello", "expected hello");
    expect(sub->active(), "expected active");
}

void test_unsubscribe() {
    observer_subscription::EventBus bus;

    int count = 0;
    auto sub = bus.subscribe("click", [&count](const observer_subscription::Event&) {
        count++;
    });

    bus.publish({"click", ""});
    expect(count == 1, "expected 1");

    sub->unsubscribe();
    expect(!sub->active(), "expected inactive");
    expect(bus.handlerCount("click") == 0, "expected 0 handlers");

    bus.publish({"click", ""});
    expect(count == 1, "expected still 1 after unsubscribe");
}

void test_move_subscription() {
    observer_subscription::EventBus bus;

    int count = 0;
    auto sub = bus.subscribe("event", [&count](const observer_subscription::Event&) {
        count++;
    });

    auto moved = std::move(sub);
    bus.publish({"event", ""});
    expect(count == 1, "expected 1");

    moved->unsubscribe();
    bus.publish({"event", ""});
    expect(count == 1, "expected still 1");
}

// Regression: a handler that unsubscribes itself while the event is
// being delivered changed the list the loop was walking. The handlers
// after it were moved out of the way, and calling one of them threw
// std::bad_function_call -- the program died.
void test_a_handler_may_unsubscribe_itself() {
    observer_subscription::EventBus bus;
    int calls = 0;

    observer_subscription::Subscription* firstPtr = nullptr;
    auto first = bus.subscribe("tick", [&](const observer_subscription::Event&) {
        calls++;
        if (firstPtr) firstPtr->unsubscribe();
    });
    firstPtr = first.get();

    auto second = bus.subscribe("tick", [&](const observer_subscription::Event&) {
        calls++;
    });

    bus.publish({"tick", "1"});
    expect(calls == 2,
           "expected both subscribers to be called, got " +
               std::to_string(calls));

    bus.publish({"tick", "2"});
    expect(calls == 3,
           "expected the second subscriber to be called again, got " +
               std::to_string(calls));

    // The first one left, the second one stayed.
    expect(bus.handlerCount("tick") == 1, "expected 1 subscriber left");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Subscription Tests:\n";
        test_subscribe_and_publish();
        std::cout << "  test_subscribe_and_publish: PASS\n";

        test_unsubscribe();
        std::cout << "  test_unsubscribe: PASS\n";

        test_move_subscription();
        std::cout << "  test_move_subscription: PASS\n";

        test_a_handler_may_unsubscribe_itself();
        std::cout << "  test_a_handler_may_unsubscribe_itself: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "observer_subscription_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nobserver_subscription_tests passed" << std::endl;
    return 0;
}
