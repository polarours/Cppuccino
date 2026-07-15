#include "active_record.hpp"

#include <iostream>

int main() {
    std::cout << "=== Active Record Demo ===\n\n";

    active_record::UserRecord alice("Alice", "alice@test.com");
    alice.save();
    std::cout << "Saved: " << alice.name() << " (id=" << alice.id() << ")\n";

    auto found = active_record::UserRecord::find(alice.id());
    if (found) {
        std::cout << "Found: " << found->name() << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
