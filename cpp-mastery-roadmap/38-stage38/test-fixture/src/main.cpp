#include "test_fixture.hpp"

#include <iostream>

int main() {
    std::cout << "=== Test Fixture Demo ===\n\n";

    test_fixture::TodoTestFixture fixture;
    std::cout << "Setup items: " << fixture.list().size() << "\n";

    fixture.list().complete(fixture.id1());
    std::cout << "Completed: " << fixture.list().completedCount() << "\n";

    fixture.teardown();
    std::cout << "After teardown: " << fixture.list().size() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
