#pragma once

#include "protocol/protocol.hpp"

#include <string>
#include <functional>

namespace key_value_store {

class Client {
public:
    Client(const std::string& host, int port);
    ~Client();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(Client&&) = delete;

    bool connect();
    void disconnect();
    bool isConnected() const;

    ProtocolResponse execute(const std::string& command);
    void interactive();

    void setEcho(bool enabled);
    bool echo() const;

private:
    bool ensureConnected();

    std::string host_;
    int port_;
    int sock_;
    bool connected_;
    bool echo_;
};

} // namespace key_value_store
