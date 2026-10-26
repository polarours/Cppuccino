#include "observer_mediator.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_notify() {
    observer_mediator::Mediator mediator;

    std::string received;
    mediator.registerHandler("click", [&received](const observer_mediator::Event& e) {
        received = e.data;
    });

    mediator.notify("button", {"click", "clicked"});
    expect(received == "clicked", "expected clicked");
}

void test_multiple_handlers() {
    observer_mediator::Mediator mediator;

    int count = 0;
    mediator.registerHandler("event", [&count](const observer_mediator::Event&) { count++; });
    mediator.registerHandler("event", [&count](const observer_mediator::Event&) { count++; });

    mediator.notify("source", {"event", ""});
    expect(count == 2, "expected 2 handlers");
}

// A handler is only called for the event type it was registered for. A
// mediator that called every handler would still pass the two tests
// above, so the routing needs a test of its own.
void test_only_the_matching_handlers_run() {
    observer_mediator::Mediator mediator;

    int clicks = 0;
    int scrolls = 0;
    mediator.registerHandler("click", [&clicks](const observer_mediator::Event&) { clicks++; });
    mediator.registerHandler("scroll", [&scrolls](const observer_mediator::Event&) { scrolls++; });

    mediator.notify("button", {"click", "clicked"});
    expect(clicks == 1, "expected the click handler to run");
    expect(scrolls == 0, "expected the scroll handler not to run");

    mediator.notify("wheel", {"scroll", "scrolled"});
    expect(clicks == 1, "expected the click handler not to run");
    expect(scrolls == 1, "expected the scroll handler to run");
}

// An event nobody listens to is not an error.
void test_notify_without_a_handler() {
    observer_mediator::Mediator mediator;

    int count = 0;
    mediator.registerHandler("click", [&count](const observer_mediator::Event&) { count++; });

    mediator.notify("button", {"scroll", "scrolled"});
    expect(count == 0, "expected no handler to run");

    // And the mediator is still usable afterwards.
    mediator.notify("button", {"click", "clicked"});
    expect(count == 1, "expected the click handler to run");
}

// The event the mediator hands over is the one the sender passed in.
void test_the_event_reaches_the_handler_unchanged() {
    observer_mediator::Mediator mediator;

    observer_mediator::Event received;
    mediator.registerHandler("click", [&received](const observer_mediator::Event& e) { received = e; });

    mediator.notify("button", {"click", "clicked"});
    expect(received.type == "click", "expected the type");
    expect(received.data == "clicked", "expected the data");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Mediator Tests:\n";
        test_basic_notify();
        std::cout << "  test_basic_notify: PASS\n";

        test_multiple_handlers();
        std::cout << "  test_multiple_handlers: PASS\n";

        test_only_the_matching_handlers_run();
        std::cout << "  test_only_the_matching_handlers_run: PASS\n";

        test_notify_without_a_handler();
        std::cout << "  test_notify_without_a_handler: PASS\n";

        test_the_event_reaches_the_handler_unchanged();
        std::cout << "  test_the_event_reaches_the_handler_unchanged: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_mediator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_mediator_tests passed" << std::endl;
    return 0;
}
