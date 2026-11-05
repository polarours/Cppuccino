#include "chat_server.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <future>

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

    server.setOnMessage([&](const chat_server::Message&) {
        messageCount++;
    });

    chat_server::Message msg{"Alice", "Hello", "", ""};
    server.broadcast(msg);

    expect(messageCount == 1,
           "expected the message callback to fire once, got " +
               std::to_string(messageCount.load()));
}

// The callback belongs to the user, and the natural thing to do in it is
// to answer the message by doing something to the server. The server used
// to call the callback while it was holding its own mutex, so a callback
// that called joinRoom() deadlocked the whole server.
void test_callback_may_call_back_into_the_server() {
    // The server is on the heap and is not deleted on purpose. If the
    // callback deadlocks, the destructor would wait for the very mutex
    // that the stuck thread is holding, and the test would hang instead
    // of failing with a message.
    auto* server = new chat_server::ChatServer(17015);
    std::atomic<int> joined{0};

    server->setOnMessage([&](const chat_server::Message&) {
        server->joinRoom("Alice", "general");
        joined++;
    });

    chat_server::Message msg{"System", "hello", "", ""};

    // The broadcast has to finish. If it deadlocks, the wait times out and
    // the test fails with a message instead of hanging the whole run.
    std::promise<void> done;
    std::future<void> finished = done.get_future();
    std::thread worker([&]() {
        server->broadcast(msg);
        done.set_value();
    });

    bool returned = finished.wait_for(std::chrono::seconds(5)) ==
                    std::future_status::ready;
    if (!returned) {
        worker.detach();
        expect(false, "the broadcast deadlocked inside the message callback");
    }
    worker.join();

    expect(joined.load() == 1, "expected the callback to run once");
    expect(server->getRoomUsers("general").size() == 1,
           "expected the callback to have put alice in the room");
    delete server;
}

// The join and leave callbacks fire when a client connects and goes away.
// The old test set them and then incremented the counters by hand, so it
// never showed that the server calls them at all.
void test_join_and_leave_callbacks() {
    chat_server::ChatServer server(17016);
    std::atomic<int> joinCount{0};
    std::atomic<int> leaveCount{0};

    server.setOnUserJoin([&](const std::string& user) {
        if (user == "Alice") joinCount++;
    });
    server.setOnUserLeave([&](const std::string& user) {
        if (user == "Alice") leaveCount++;
    });

    std::thread serverThread([&server]() {
        server.start();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    expect(fd >= 0, "expected socket creation to succeed");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(17016);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bool connected = connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0;
    expect(connected, "expected client connect to succeed");
    send(fd, "Alice\n", 6, MSG_NOSIGNAL);

    // The server reads the name and calls the callback on its own thread.
    // A fixed sleep would be a race on a machine that is busy, so the
    // count is waited for instead.
    for (int i = 0; i < 500 && joinCount.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    int joins = joinCount.load();

    // Closing the socket has to make the server notice the client is gone.
    close(fd);
    for (int i = 0; i < 500 && leaveCount.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    int leaves = leaveCount.load();

    // The server is stopped before the checks: an expect that throws while
    // a thread is still joinable ends the program with "terminate called"
    // instead of a message that says what went wrong.
    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }

    expect(joins == 1,
           "expected the join callback to fire once, got " +
               std::to_string(joins));
    expect(leaves == 1,
           "expected the leave callback to fire once, got " +
               std::to_string(leaves));
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

// Regression: stop() must terminate even though the accept loop and the
// client handler are both blocked in a socket read. close() alone does not
// wake a blocked accept()/recv() on Linux, so stop() has to shut the
// sockets down explicitly. Without this, stop() hangs and never returns.
void test_stop_terminates_with_blocked_accept() {
    chat_server::ChatServer server(17013);

    std::thread serverThread([&server]() {
        server.start();
    });

    // Give the accept loop time to block.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::atomic<bool> stopped{false};
    std::thread stopper([&server, &stopped]() {
        server.stop();
        stopped = true;
    });

    // If stop() hangs, the join below never returns and the test times out
    // instead of failing cleanly.
    stopper.join();
    expect(stopped.load(), "expected stop() to complete");

    if (serverThread.joinable()) {
        serverThread.join();
    }
}

// Regression: a client that stays connected without sending anything leaves
// its handler blocked in recv(). stop() must still return.
void test_stop_terminates_with_idle_client() {
    chat_server::ChatServer server(17014);

    std::thread serverThread([&server]() {
        server.start();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Connect a client and leave it idle.
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    expect(fd >= 0, "expected socket creation to succeed");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(17014);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bool connected = connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0;
    expect(connected, "expected client connect to succeed");
    send(fd, "Alice\n", 6, MSG_NOSIGNAL);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    server.stop();
    expect(!server.isRunning(), "expected server not running after stop");

    if (serverThread.joinable()) {
        serverThread.join();
    }
    close(fd);
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

        test_callback_may_call_back_into_the_server();
        std::cout << "  test_callback_may_call_back_into_the_server: PASS\n";

        test_join_and_leave_callbacks();
        std::cout << "  test_join_and_leave_callbacks: PASS\n";

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

        test_stop_terminates_with_blocked_accept();
        std::cout << "  test_stop_terminates_with_blocked_accept: PASS\n";

        test_stop_terminates_with_idle_client();
        std::cout << "  test_stop_terminates_with_idle_client: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "chat_server_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nchat_server_tests passed" << std::endl;
    return 0;
}
