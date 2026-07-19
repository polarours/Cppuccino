#include "deducing_this.hpp"

#include <iostream>

int main() {
    std::cout << "=== Deducing This Demo ===\n\n";

    deducing_this::Builder builder;
    auto person = builder.withName("Alice").withAge(30).withEmail("alice@test.com");
    std::cout << person.describe() << "\n";

    deducing_this::Counter counter;
    auto result = counter.increment().add(10).increment();
    std::cout << "Counter: " << result.value() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
