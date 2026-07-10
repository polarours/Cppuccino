#include "mediator_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Mediator Pattern Demo ===\n\n";

    auto chatRoom = std::make_shared<mediator_pattern::ChatRoom>();

    auto alice = std::make_shared<mediator_pattern::User>(chatRoom, "Alice");
    auto bob = std::make_shared<mediator_pattern::User>(chatRoom, "Bob");
    auto charlie = std::make_shared<mediator_pattern::User>(chatRoom, "Charlie");

    chatRoom->addColleague(alice);
    chatRoom->addColleague(bob);
    chatRoom->addColleague(charlie);

    alice->send("Hello everyone!");
    bob->send("Hi Alice!");
    charlie->send("Hey there!");

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
