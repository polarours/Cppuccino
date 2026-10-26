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

// The stream can be asked for the events from a version on. The version
// is included, so a caller that has applied version 3 asks for 4.
void test_get_events_since() {
    event_sourcing::EventStore store;

    store.append({"Created", "100"});
    store.append({"Deposited", "50"});
    store.append({"Withdrawn", "20"});

    expect(store.version() == 3, "expected version 3");

    const auto all = store.getEvents();
    expect(all.size() == 3, "expected 3 events");

    const auto sinceTwo = store.getEventsSince(2);
    expect(sinceTwo.size() == 2, "expected 2 events since version 2");
    expect(sinceTwo[0].version == 2, "expected version 2 first");
    expect(sinceTwo[1].version == 3, "expected version 3 second");

    // A version that is past the end gives nothing back.
    expect(store.getEventsSince(4).empty(), "expected no events");
    expect(store.getEventsSince(0).size() == 3, "expected all 3 events");
}

// clear() empties the stream and starts the versions over.
void test_clear() {
    event_sourcing::EventStore store;

    store.append({"Created", "100"});
    store.append({"Deposited", "50"});
    expect(store.size() == 2, "expected 2 events");

    store.clear();
    expect(store.size() == 0, "expected no events");
    expect(store.version() == 0, "expected version 0");
    expect(store.getEvents().empty(), "expected no events");

    // The next event starts at version 1 again.
    store.append({"Created", "200"});
    expect(store.version() == 1, "expected version 1");
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

        test_get_events_since();
        std::cout << "  test_get_events_since: PASS\n";

        test_clear();
        std::cout << "  test_clear: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "event_sourcing_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nevent_sourcing_tests passed" << std::endl;
    return 0;
}
