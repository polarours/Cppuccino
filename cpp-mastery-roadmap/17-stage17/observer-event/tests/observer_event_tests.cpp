#include "observer_event.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_emit() {
    observer_event::EventDispatcher dispatcher;

    std::string received;
    dispatcher.on("click", [&received](const observer_event::Event& e) {
        received = e.data;
    });

    dispatcher.emit({"click", "button", "clicked"});
    expect(received == "clicked", "expected clicked");
}

void test_multiple_handlers() {
    observer_event::EventDispatcher dispatcher;

    int count = 0;
    dispatcher.on("event", [&count](const observer_event::Event&) { count++; });
    dispatcher.on("event", [&count](const observer_event::Event&) { count++; });

    dispatcher.emit({"event", "source", ""});
    expect(count == 2, "expected 2 handlers");
}

void test_different_events() {
    observer_event::EventDispatcher dispatcher;

    std::string clickEvent, hoverEvent;
    dispatcher.on("click", [&clickEvent](const observer_event::Event& e) { clickEvent = e.data; });
    dispatcher.on("hover", [&hoverEvent](const observer_event::Event& e) { hoverEvent = e.data; });

    dispatcher.emit({"click", "btn", "clicked"});
    dispatcher.emit({"hover", "btn", "hovered"});

    expect(clickEvent == "clicked", "expected clicked");
    expect(hoverEvent == "hovered", "expected hovered");
}

void test_handler_count() {
    observer_event::EventDispatcher dispatcher;

    dispatcher.on("event1", [](const observer_event::Event&) {});
    dispatcher.on("event1", [](const observer_event::Event&) {});
    dispatcher.on("event2", [](const observer_event::Event&) {});

    expect(dispatcher.handlerCount("event1") == 2, "expected 2 handlers for event1");
    expect(dispatcher.handlerCount("event2") == 1, "expected 1 handler for event2");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Event Tests:\n";
        test_basic_emit();
        std::cout << "  test_basic_emit: PASS\n";

        test_multiple_handlers();
        std::cout << "  test_multiple_handlers: PASS\n";

        test_different_events();
        std::cout << "  test_different_events: PASS\n";

        test_handler_count();
        std::cout << "  test_handler_count: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_event_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_event_tests passed" << std::endl;
    return 0;
}
