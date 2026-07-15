#include "event_sourcing.hpp"

#include <iostream>

int main() {
    std::cout << "=== Event Sourcing Demo ===\n\n";

    event_sourcing::EventStore store;
    event_sourcing::AccountAggregate account(store);

    account.create("Alice");
    account.deposit(100.0);
    account.withdraw(30.0);

    std::cout << account.account().name() << ": $" << account.account().balance() << "\n";
    std::cout << "Events: " << store.size() << ", Version: " << store.version() << "\n";

    event_sourcing::AccountAggregate rebuilt(store);
    rebuilt.rebuild();
    std::cout << "Rebuilt: " << rebuilt.account().name() << ": $" << rebuilt.account().balance() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
