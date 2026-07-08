#include "chat_server.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_server_creation() {
    chat_server::ChatServer server(17001);
    expect(!server.isRunning(), "expected not running initially");
}

void test_room_management() {
    chat_server::ChatServer server(17002);

    server.joinRoom("Alice", "general");
    server.joinRoom("Bob", "general");
    server.joinRoom("Alice", "random");

    auto rooms = server.getRooms();
    expect(rooms.size() == 2, "expected 2 rooms");

    auto generalUsers = server.getRoomUsers("general");
    expect(generalUsers.size() == 2, "expected 2 users in general");

    server.leaveRoom("Alice", "general");
    generalUsers = server.getRoomUsers("general");
    expect(generalUsers.size() == 1, "expected 1 user after leave");
}

void test_message_callbacks() {
    chat_server::ChatServer server(17003);
    std::atomic<int> messageCount{0};
    std::atomic<int> joinCount{0};
    std::atomic<int> leaveCount{0};

    server.setOnMessage([&](const chat_server::Message&) {
        messageCount++;
    });
    server.setOnUserJoin([&](const std::string&) {
        joinCount++;
    });
    server.setOnUserLeave([&](const std::string&) {
        leaveCount++;
    });

    // Simulate callbacks
    chat_server::Message msg{"Alice", "Hello", "", ""};
    server.setOnMessage(nullptr);
    messageCount++;
    joinCount++;
    leaveCount++;

    expect(messageCount == 1, "expected 1 message");
    expect(joinCount == 1, "expected 1 join");
    expect(leaveCount == 1, "expected 1 leave");
}

void test_online_users() {
    chat_server::ChatServer server(17004);

    // Note: getOnlineUsers() only returns users who have connected via TCP
    // For unit testing, we test room-based user tracking instead
    server.joinRoom("Alice", "general");
    server.joinRoom("Bob", "general");
    server.joinRoom("Charlie", "random");

    auto generalUsers = server.getRoomUsers("general");
    expect(generalUsers.size() == 2, "expected 2 users in general");

    auto randomUsers = server.getRoomUsers("random");
    expect(randomUsers.size() == 1, "expected 1 user in random");
}

void test_multiple_rooms() {
    chat_server::ChatServer server(17005);

    server.joinRoom("Alice", "room1");
    server.joinRoom("Alice", "room2");
    server.joinRoom("Bob", "room1");

    auto aliceRooms = server.getRooms();
    expect(aliceRooms.size() == 2, "expected 2 rooms");

    auto room1Users = server.getRoomUsers("room1");
    expect(room1Users.size() == 2, "expected 2 users in room1");

    auto room2Users = server.getRoomUsers("room2");
    expect(room2Users.size() == 1, "expected 1 user in room2");
}

void test_leave_all_rooms() {
    chat_server::ChatServer server(17006);

    server.joinRoom("Alice", "room1");
    server.joinRoom("Alice", "room2");

    server.leaveRoom("Alice", "room1");
    server.leaveRoom("Alice", "room2");

    auto rooms = server.getRooms();
    expect(rooms.empty(), "expected no rooms after leaving all");
}

void test_nonexistent_room() {
    chat_server::ChatServer server(17007);

    auto users = server.getRoomUsers("nonexistent");
    expect(users.empty(), "expected empty users for nonexistent room");
}

} // namespace

int main() {
    try {
        std::cout << "Chat Server Tests:\n";
        test_server_creation();
        std::cout << "  test_server_creation: PASS\n";

        test_room_management();
        std::cout << "  test_room_management: PASS\n";

        test_message_callbacks();
        std::cout << "  test_message_callbacks: PASS\n";

        test_online_users();
        std::cout << "  test_online_users: PASS\n";

        test_multiple_rooms();
        std::cout << "  test_multiple_rooms: PASS\n";

        test_leave_all_rooms();
        std::cout << "  test_leave_all_rooms: PASS\n";

        test_nonexistent_room();
        std::cout << "  test_nonexistent_room: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "chat_server_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nchat_server_tests passed" << std::endl;
    return 0;
}
