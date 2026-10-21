# Chat Server API

## Overview

`chat-server` is a multithreaded TCP chat server that supports room management,
message broadcasting and user commands.

## Core Types

### Message

```cpp
struct Message {
    std::string sender;   // sender username
    std::string content;  // message body
    std::string room;     // target room
    std::string timestamp; // timestamp
};
```

## ChatServer Class API

### Construction and Lifecycle

| Method | Signature | Description |
|---|---|---|
| Constructor | `ChatServer(int port)` | Binds to the given port |
| `start` | `void start()` | Starts the server thread |
| `stop` | `void stop()` | Stops the server and waits for every client thread to finish |
| `isRunning` | `bool isRunning() const` | Reports whether the server is running |

### Room Management

| Method | Signature | Description |
|---|---|---|
| `joinRoom` | `void joinRoom(const std::string& user, const std::string& room)` | Adds a user to a room |
| `leaveRoom` | `void leaveRoom(const std::string& user, const std::string& room)` | Removes a user from a room |
| `getRooms` | `std::vector<std::string> getRooms() const` | Lists every room |
| `getRoomUsers` | `std::vector<std::string> getRoomUsers(const std::string& room) const` | Lists the members of a room |
| `getOnlineUsers` | `std::vector<std::string> getOnlineUsers() const` | Lists every connected user |

### Sending Messages

| Method | Signature | Description |
|---|---|---|
| `broadcast` | `void broadcast(const Message& msg)` | Sends a message to every user |
| `sendToUser` | `void sendToUser(const std::string& user, const Message& msg)` | Sends a message to one user |
| `sendToRoom` | `void sendToRoom(const std::string& room, const Message& msg)` | Sends a message to every user in a room |

### Event Callbacks

| Method | Signature | Description |
|---|---|---|
| `setOnMessage` | `void setOnMessage(std::function<void(const Message&)>)` | Registers the message callback |
| `setOnUserJoin` | `void setOnUserJoin(std::function<void(const std::string&)>)` | Registers the user-joined callback |
| `setOnUserLeave` | `void setOnUserLeave(std::function<void(const std::string&)>)` | Registers the user-left callback |

### Internal Handling

`processCommand` is private. It is reached from a connected client when the
client sends a line that starts with `/`, so the command table below is
observable over TCP rather than through the public API.

| Method | Signature | Description |
|---|---|---|
| `processCommand` | `void processCommand(const std::string& user, const std::string& cmd)` | Dispatches a user command |

## Supported Commands

| Command | Format | Description |
|---|---|---|
| `/join` | `/join <room>` | Joins a room |
| `/leave` | `/leave <room>` | Leaves a room |
| `/rooms` | `/rooms` | Lists every room |
| `/users` | `/users` | Lists the connected users |
| `/quit` | `/quit` | Disconnects from the server |

## Thread Safety

- Shared state is protected by a `std::mutex`; the public query and send
  methods take the lock before reading or mutating it.
- `clientThreads_` owns one connection thread per client. `stop()` joins every
  thread before it returns, so the vector is not mutated concurrently with a
  running client.
- Broadcasting and room operations are atomic with respect to other callers
  because each of them holds the lock for the whole operation.

## Complete Example

The example drives the server over a real TCP connection, because the command
surface is only reachable through a connected client.

```cpp
#include "chat_server.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

// Connects to the server, sends one line and collects the reply.
std::vector<std::string> talk(int port, const std::string& user,
                              const std::vector<std::string>& lines) {
    int fd = -1;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // start() binds and listens on its own thread, so retry until the
    // listening socket is actually accepting connections.
    for (int attempt = 0; attempt < 50; ++attempt) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            return {};
        }
        if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
            break;
        }
        close(fd);
        fd = -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (fd < 0) {
        return {};
    }

    auto sendLine = [&](const std::string& text) {
        send(fd, (text + "\n").c_str(), text.size() + 1, 0);
    };
    sendLine(user);  // the first line is the username

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    for (const auto& line : lines) {
        sendLine(line);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::string buffer(4096, '\0');
    std::vector<std::string> replies;
    ssize_t bytes = recv(fd, buffer.data(), buffer.size() - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::string reply(buffer.data());
        size_t start = 0;
        while (start < reply.size()) {
            size_t end = reply.find('\n', start);
            if (end == std::string::npos) end = reply.size();
            if (end > start) replies.push_back(reply.substr(start, end - start));
            start = end + 1;
        }
    }
    close(fd);
    return replies;
}

int main(int argc, char** argv) {
    // The port is configurable because 8080 is often already taken.
    const int port = argc > 1 ? std::atoi(argv[1]) : 8080;

    chat_server::ChatServer server(port);

    // Register event callbacks.
    server.setOnUserJoin([](const std::string& user) {
        std::cout << user << " joined the server\n";
    });
    server.setOnUserLeave([](const std::string& user) {
        std::cout << user << " left the server\n";
    });

    // start() blocks in an accept loop, so it runs on its own thread.
    std::thread serverThread([&server]() {
        server.start();
    });

    // Drive the command surface as a real client would.
    for (const auto& reply : talk(port, "Alice", {"/join general", "/rooms"})) {
        std::cout << reply << "\n";
    }

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
    return 0;
}
```

## Build and Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
