#include "mock_objects.hpp"

#include <iostream>

int main() {
    std::cout << "=== Mock Objects Demo ===\n\n";

    mock_objects::MockDatabase mock;
    mock.setLastQueryResult("Alice:30");

    mock_objects::UserRepository repo(mock);
    repo.initialize("localhost");

    std::string result;
    repo.findUser("Alice", result);
    std::cout << "Query result: " << result << "\n";

    repo.shutdown();
    std::cout << "Disconnect called: " << std::boolalpha << mock.disconnectCalled() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
