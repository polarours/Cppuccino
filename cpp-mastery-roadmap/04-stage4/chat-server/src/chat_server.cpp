#include "chat_server.hpp"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>

namespace chat_server {

ChatServer::ChatServer(int port)
    : port_(port)
    , server_fd_(-1)
    , running_(false) {}

ChatServer::~ChatServer() {
    stop();
}

void ChatServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int reuse = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to bind");
    }

    if (listen(server_fd_, 10) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to listen");
    }

    running_ = true;
    std::cout << "Chat server started on port " << port_ << "\n";

    while (running_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            if (!running_) break;
            continue;
        }

        clientThreads_.emplace_back(&ChatServer::handleClient, this, client_fd);
    }
}

void ChatServer::stop() {
    running_ = false;
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }

    for (auto& thread : clientThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    clientThreads_.clear();
}

void ChatServer::handleClient(int client_fd) {
    char buffer[4096];
    std::string username;

    // Expect first message to be username
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    username = buffer;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        users_[username] = client_fd;
    }

    if (onUserJoin_) {
        onUserJoin_(username);
    }

    Message joinMsg;
    joinMsg.sender = "System";
    joinMsg.content = username + " has joined the chat";
    broadcast(joinMsg);

    std::string welcome = "Welcome, " + username + "! Commands: /join <room>, /leave <room>, /rooms, /users, /quit\n";
    send(client_fd, welcome.c_str(), welcome.size(), 0);

    while (running_) {
        bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';
        std::string msg(buffer);

        if (msg == "/quit") break;

        if (msg[0] == '/') {
            processCommand(username, msg);
        } else {
            Message chatMsg;
            chatMsg.sender = username;
            chatMsg.content = msg;
            broadcast(chatMsg);
        }
    }

    Message leaveMsg;
    leaveMsg.sender = "System";
    leaveMsg.content = username + " has left the chat";
    broadcast(leaveMsg);

    if (onUserLeave_) {
        onUserLeave_(username);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        users_.erase(username);
        for (auto& [room, members] : userRooms_) {
            members.erase(username);
        }
    }

    close(client_fd);
}

void ChatServer::processCommand(const std::string& user, const std::string& cmd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;

    if (command == "/join") {
        std::string room;
        stream >> room;
        if (!room.empty()) {
            joinRoom(user, room);
            Message msg;
            msg.sender = "System";
            msg.content = user + " joined room " + room;
            sendToRoom(room, msg);
        }
    } else if (command == "/leave") {
        std::string room;
        stream >> room;
        if (!room.empty()) {
            leaveRoom(user, room);
            Message msg;
            msg.sender = "System";
            msg.content = user + " left room " + room;
            sendToRoom(room, msg);
        }
    } else if (command == "/rooms") {
        auto rooms = getRooms();
        std::string response = "Rooms: ";
        for (const auto& room : rooms) {
            response += room + " ";
        }
        Message msg;
        msg.sender = "System";
        msg.content = response;
        sendToUser(user, msg);
    } else if (command == "/users") {
        auto users = getOnlineUsers();
        std::string response = "Online users: ";
        for (const auto& u : users) {
            response += u + " ";
        }
        Message msg;
        msg.sender = "System";
        msg.content = response;
        sendToUser(user, msg);
    }
}

void ChatServer::broadcast(const Message& msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string formatted = "[" + msg.sender + "] " + msg.content + "\n";

    for (const auto& [user, fd] : users_) {
        send(fd, formatted.c_str(), formatted.size(), 0);
    }

    if (onMessage_) {
        onMessage_(msg);
    }
}

void ChatServer::sendToUser(const std::string& user, const Message& msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = users_.find(user);
    if (it != users_.end()) {
        std::string formatted = "[" + msg.sender + "] " + msg.content + "\n";
        send(it->second, formatted.c_str(), formatted.size(), 0);
    }
}

void ChatServer::sendToRoom(const std::string& room, const Message& msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto roomIt = rooms_.find(room);
    if (roomIt == rooms_.end()) return;

    std::string formatted = "[" + msg.sender + " #" + room + "] " + msg.content + "\n";
    for (const auto& user : roomIt->second) {
        auto userIt = users_.find(user);
        if (userIt != users_.end()) {
            send(userIt->second, formatted.c_str(), formatted.size(), 0);
        }
    }
}

void ChatServer::joinRoom(const std::string& user, const std::string& room) {
    std::lock_guard<std::mutex> lock(mutex_);
    rooms_[room].insert(user);
    userRooms_[user].insert(room);
}

void ChatServer::leaveRoom(const std::string& user, const std::string& room) {
    std::lock_guard<std::mutex> lock(mutex_);
    rooms_[room].erase(user);
    userRooms_[user].erase(room);
    if (rooms_[room].empty()) {
        rooms_.erase(room);
    }
}

std::vector<std::string> ChatServer::getOnlineUsers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> users;
    for (const auto& [user, fd] : users_) {
        users.push_back(user);
    }
    return users;
}

std::vector<std::string> ChatServer::getRooms() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> rooms;
    for (const auto& [room, members] : rooms_) {
        rooms.push_back(room);
    }
    return rooms;
}

std::vector<std::string> ChatServer::getRoomUsers(const std::string& room) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> users;
    auto it = rooms_.find(room);
    if (it != rooms_.end()) {
        for (const auto& user : it->second) {
            users.push_back(user);
        }
    }
    return users;
}

void ChatServer::setOnMessage(std::function<void(const Message&)> callback) {
    onMessage_ = std::move(callback);
}

void ChatServer::setOnUserJoin(std::function<void(const std::string&)> callback) {
    onUserJoin_ = std::move(callback);
}

void ChatServer::setOnUserLeave(std::function<void(const std::string&)> callback) {
    onUserLeave_ = std::move(callback);
}

bool ChatServer::isRunning() const {
    return running_;
}

} // namespace chat_server
