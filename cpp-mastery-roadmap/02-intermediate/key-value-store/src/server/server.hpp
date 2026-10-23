#pragma once

#include "store/store_manager.hpp"
#include "protocol/protocol.hpp"

#include <string>
#include <thread>
#include <vector>
#include <set>
#include <atomic>
#include <mutex>

namespace key_value_store {

class Server {
public:
    Server(const std::string& host, int port, const std::string& persistencePath);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void start();
    void stop();

    bool isRunning() const;

private:
    void handleClient(int client_fd);
    int setupSocket();

    std::string host_;
    int port_;
    // The listening socket. start() creates it and closes it again when the
    // accept loop ends; stop() only shuts it down, so that a thread blocked
    // in accept() wakes up. Every access is under mutex_.
    int server_fd_;
    StoreManager storeManager_;
    Protocol protocol_;
    std::atomic<bool> running_;
    // The threads that serve the clients. The accept loop fills the list
    // and stop() takes it over, both under mutex_. The threads themselves
    // are joined outside of the lock.
    std::vector<std::thread> clientThreads_;
    // The client sockets that are still being served. stop() shuts them down
    // so that a thread blocked in read() wakes up instead of hanging the
    // join.
    std::set<int> clientFds_;
    std::mutex mutex_;
};

} // namespace key_value_store
