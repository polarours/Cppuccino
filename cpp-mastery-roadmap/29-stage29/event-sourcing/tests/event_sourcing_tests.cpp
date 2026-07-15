#include "event_sourcing.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void expectNear(double a, double b, const std::string& message) {
    if (std::abs(a - b) > 0.01) {
        throw std::runtime_error(message);
    }
}

void test_create_events() {
    event_sourcing::EventStore store;
    event_sourcing::AccountAggregate account(store);

    account.create("Alice");
    expect(store.size() == 1, "expected 1 event");
    expect(account.account().name() == "Alice", "expected Alice");
}

void test_deposit_withdraw() {
    event_sourcing::EventStore store;
    event_sourcing::AccountAggregate account(store);

    account.create("Alice");
    account.deposit(100.0);
    account.withdraw(30.0);

    expectNear(account.account().balance(), 70.0, "expected 70 balance");
    expect(store.size() == 3, "expected 3 events");
}

void test_rebuild_from_events() {
    event_sourcing::EventStore store;
    event_sourcing::AccountAggregate account(store);

    account.create("Alice");
    account.deposit(200.0);
    account.withdraw(50.0);

    event_sourcing::AccountAggregate rebuilt(store);
    rebuilt.rebuild();

    expect(rebuilt.account().name() == "Alice", "expected Alice after rebuild");
    expectNear(rebuilt.account().balance(), 150.0, "expected 150 after rebuild");
}

void test_version_tracking() {
    event_sourcing::EventStore store;
    event_sourcing::AccountAggregate account(store);

    account.create("Bob");
    expect(store.version() == 1, "expected version 1");

    account.deposit(50.0);
    expect(store.version() == 2, "expected version 2");
}

} // namespace

int main() {
    try {
        std::cout << "Event Sourcing Tests:\n";
        test_create_events();
        std::cout << "  test_create_events: PASS\n";

        test_deposit_withdraw();
        std::cout << "  test_deposit_withdraw: PASS\n";

        test_rebuild_from_events();
        std::cout << "  test_rebuild_from_events: PASS\n";

        test_version_tracking();
        std::cout << "  test_version_tracking: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "event_sourcing_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nevent_sourcing_tests passed" << std::endl;
    return 0;
}
