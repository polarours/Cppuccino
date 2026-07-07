#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <sstream>

namespace chat_server {

struct Message {
    std::string sender;
    std::string content;
    std::string room;
    std::string timestamp;
};

class ChatServer {
public:
    ChatServer(int port);
    ~ChatServer();

    void start();
    void stop();

    void broadcast(const Message& msg);
    void sendToUser(const std::string& user, const Message& msg);
    void sendToRoom(const std::string& room, const Message& msg);

    void joinRoom(const std::string& user, const std::string& room);
    void leaveRoom(const std::string& user, const std::string& room);

    std::vector<std::string> getOnlineUsers() const;
    std::vector<std::string> getRooms() const;
    std::vector<std::string> getRoomUsers(const std::string& room) const;

    void setOnMessage(std::function<void(const Message&)> callback);
    void setOnUserJoin(std::function<void(const std::string&)> callback);
    void setOnUserLeave(std::function<void(const std::string&)> callback);

    bool isRunning() const;

private:
    void handleClient(int client_fd);
    void processCommand(const std::string& user, const std::string& cmd);

    int port_;
    int server_fd_;
    std::atomic<bool> running_;
    std::map<std::string, int> users_;
    std::map<std::string, std::set<std::string>> rooms_;
    std::map<std::string, std::set<std::string>> userRooms_;
    std::vector<std::thread> clientThreads_;

    std::function<void(const Message&)> onMessage_;
    std::function<void(const std::string&)> onUserJoin_;
    std::function<void(const std::string&)> onUserLeave_;

    mutable std::mutex mutex_;
};

} // namespace chat_server
