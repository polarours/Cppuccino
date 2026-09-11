// examples/mediator-pattern-demo.cpp
// Demonstrates Mediator Pattern: ChatRoom mediates between Users.
// Compile: g++ -std=c++20 -o mediator-pattern-demo mediator-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace mediator_pattern {

class ChatRoom;

class User {
public:
    User(std::shared_ptr<ChatRoom> room, std::string name)
        : room_(room), name_(std::move(name)) {}

    void send(const std::string& message);  // forward-declared impl below

    void receive(const std::string& from, const std::string& message) {
        std::cout << "  " << name_ << "收到来自" << from << ": " << message << "\n";
    }

    const std::string& getName() const { return name_; }

private:
    std::shared_ptr<ChatRoom> room_;
    std::string name_;
};

class ChatRoom {
public:
    void addColleague(std::shared_ptr<User> user) {
        colleagues_.push_back(user);
    }

    void broadcast(const std::string& message, const std::string& senderName) {
        std::cout << "[" << senderName << "广播] " << message << "\n";
        for (const auto& user : colleagues_) {
            if (user->getName() != senderName)
                user->receive(senderName, message);
        }
    }

private:
    std::vector<std::shared_ptr<User>> colleagues_;
};

void User::send(const std::string& message) {
    room_->broadcast(message, name_);
}

} // namespace mediator_pattern

int main() {
    using namespace mediator_pattern;
    std::cout << "=== Mediator Pattern Demo ===\n\n";

    auto chatRoom = std::make_shared<ChatRoom>();

    auto alice  = std::make_shared<User>(chatRoom, "Alice");
    auto bob    = std::make_shared<User>(chatRoom, "Bob");
    auto charlie = std::make_shared<User>(chatRoom, "Charlie");

    chatRoom->addColleague(alice);
    chatRoom->addColleague(bob);
    chatRoom->addColleague(charlie);

    alice->send("Hello everyone!");
    bob->send("Hi Alice!");
    charlie->send("Hey there!");

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
