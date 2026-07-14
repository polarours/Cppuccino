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

} // namespace

int main() {
    try {
        std::cout << "Observer Mediator Tests:\n";
        test_basic_notify();
        std::cout << "  test_basic_notify: PASS\n";

        test_multiple_handlers();
        std::cout << "  test_multiple_handlers: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_mediator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_mediator_tests passed" << std::endl;
    return 0;
}
