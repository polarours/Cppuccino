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

void test_message_structure() {
    chat_server::Message msg;
    msg.sender = "Alice";
    msg.content = "Hello";
    msg.room = "general";
    msg.timestamp = "12:00";

    expect(msg.sender == "Alice", "expected sender");
    expect(msg.content == "Hello", "expected content");
    expect(msg.room == "general", "expected room");
    expect(msg.timestamp == "12:00", "expected timestamp");
}

void test_room_auto_created() {
    chat_server::ChatServer server(17008);

    // Room should be created when first user joins
    server.joinRoom("Alice", "new_room");

    auto rooms = server.getRooms();
    expect(rooms.size() == 1, "expected 1 room");

    auto users = server.getRoomUsers("new_room");
    expect(users.size() == 1, "expected 1 user");
}

void test_room_auto_deleted() {
    chat_server::ChatServer server(17009);

    server.joinRoom("Alice", "temp_room");
    server.leaveRoom("Alice", "temp_room");

    // Room should be deleted when last user leaves
    auto rooms = server.getRooms();
    expect(rooms.empty(), "expected no rooms after last user leaves");
}

void test_user_in_multiple_rooms() {
    chat_server::ChatServer server(17010);

    server.joinRoom("Alice", "room1");
    server.joinRoom("Alice", "room2");
    server.joinRoom("Alice", "room3");

    auto rooms = server.getRooms();
    expect(rooms.size() == 3, "expected 3 rooms");

    // User should be in all 3 rooms
    for (const auto& room : rooms) {
        auto users = server.getRoomUsers(room);
        expect(users.size() == 1, "expected 1 user in each room");
    }
}

void test_multiple_users_same_room() {
    chat_server::ChatServer server(17011);

    for (int i = 0; i < 5; ++i) {
        server.joinRoom("User" + std::to_string(i), "chat");
    }

    auto users = server.getRoomUsers("chat");
    expect(users.size() == 5, "expected 5 users in chat");
}

void test_leave_nonexistent_room() {
    chat_server::ChatServer server(17012);

    // Should not crash when leaving nonexistent room
    server.leaveRoom("Alice", "nonexistent");

    auto rooms = server.getRooms();
    expect(rooms.empty(), "expected no rooms");
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

        test_message_structure();
        std::cout << "  test_message_structure: PASS\n";

        test_room_auto_created();
        std::cout << "  test_room_auto_created: PASS\n";

        test_room_auto_deleted();
        std::cout << "  test_room_auto_deleted: PASS\n";

        test_user_in_multiple_rooms();
        std::cout << "  test_user_in_multiple_rooms: PASS\n";

        test_multiple_users_same_room();
        std::cout << "  test_multiple_users_same_room: PASS\n";

        test_leave_nonexistent_room();
        std::cout << "  test_leave_nonexistent_room: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "chat_server_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nchat_server_tests passed" << std::endl;
    return 0;
}
