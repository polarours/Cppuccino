#pragma once

#include "store/store_manager.hpp"
#include "protocol/protocol.hpp"

#include <string>
#include <thread>
#include <vector>
#include <atomic>

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
    void setupSocket();
    void cleanup();

    std::string host_;
    int port_;
    int server_fd_;
    StoreManager storeManager_;
    Protocol protocol_;
    std::atomic<bool> running_;
    std::vector<std::thread> clientThreads_;
};

} // namespace key_value_store
