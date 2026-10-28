#include "mediator_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <sstream>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// The classes under test report what they do by printing, so the output
// is what the test looks at.
class CapturedOutput {
public:
    CapturedOutput() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CapturedOutput() { std::cout.rdbuf(previous_); }

    CapturedOutput(const CapturedOutput&) = delete;
    CapturedOutput& operator=(const CapturedOutput&) = delete;

    std::string text() const { return buffer_.str(); }

private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

void test_basic_communication() {
    auto chatRoom = std::make_shared<mediator_pattern::ChatRoom>();

    auto user1 = std::make_shared<mediator_pattern::User>(chatRoom, "Alice");
    auto user2 = std::make_shared<mediator_pattern::User>(chatRoom, "Bob");

    chatRoom->addColleague(user1);
    chatRoom->addColleague(user2);

    CapturedOutput output;
    user1->send("Hello!");

    const std::string text = output.text();
    expect(text.find("[Alice -> Bob] Hello!") != std::string::npos,
           "expected bob to be told about the message, got: " + text);
    expect(text.find("[Alice -> Alice]") == std::string::npos,
           "expected alice not to be told about her own message: " + text);
}

void test_multiple_users() {
    auto chatRoom = std::make_shared<mediator_pattern::ChatRoom>();

    auto user1 = std::make_shared<mediator_pattern::User>(chatRoom, "Alice");
    auto user2 = std::make_shared<mediator_pattern::User>(chatRoom, "Bob");
    auto user3 = std::make_shared<mediator_pattern::User>(chatRoom, "Charlie");

    chatRoom->addColleague(user1);
    chatRoom->addColleague(user2);
    chatRoom->addColleague(user3);

    // Should not throw
    user2->send("Hello everyone!");
}

void test_mediator_notification() {
    auto chatRoom = std::make_shared<mediator_pattern::ChatRoom>();

    auto user1 = std::make_shared<mediator_pattern::User>(chatRoom, "Alice");
    chatRoom->addColleague(user1);

    // Notify with event
    chatRoom->notify("Alice", "Test event");
}

} // namespace

int main() {
    try {
        std::cout << "Mediator Pattern Tests:\n";
        test_basic_communication();
        std::cout << "  test_basic_communication: PASS\n";

        test_multiple_users();
        std::cout << "  test_multiple_users: PASS\n";

        test_mediator_notification();
        std::cout << "  test_mediator_notification: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "mediator_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nmediator_pattern_tests passed" << std::endl;
    return 0;
}
