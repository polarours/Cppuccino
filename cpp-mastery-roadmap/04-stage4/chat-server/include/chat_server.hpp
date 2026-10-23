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
    // The listening socket. start() creates it and closes it again when the
    // accept loop ends; stop() only shuts it down, so that a thread blocked
    // in accept() wakes up. Every access is under mutex_.
    int server_fd_;
    std::atomic<bool> running_;
    std::map<std::string, int> users_;
    std::map<std::string, std::set<std::string>> rooms_;
    std::map<std::string, std::set<std::string>> userRooms_;
    // The threads that serve the clients. The accept loop fills the list
    // and stop() takes it over, both under mutex_. The threads themselves
    // are joined outside of the lock, because a client thread needs the
    // lock to broadcast its farewell message.
    std::vector<std::thread> clientThreads_;
    // Client sockets that are still being served. stop() shuts them down so
    // that threads blocked in recv() wake up instead of hanging the join.
    std::set<int> clientFds_;

    std::function<void(const Message&)> onMessage_;
    std::function<void(const std::string&)> onUserJoin_;
    std::function<void(const std::string&)> onUserLeave_;

    mutable std::mutex mutex_;
};

} // namespace chat_server
