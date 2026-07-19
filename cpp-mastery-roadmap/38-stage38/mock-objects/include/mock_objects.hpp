#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace mock_objects {

class Database {
public:
    virtual ~Database() = default;
    virtual bool connect(const std::string& url) = 0;
    virtual bool query(const std::string& sql, std::string& result) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
};

class MockDatabase : public Database {
public:
    bool connect(const std::string& url) override {
        connectCalls_.push_back(url);
        return shouldSucceed_;
    }

    bool query(const std::string& sql, std::string& result) override {
        queryCalls_.push_back(sql);
        if (queryHandler_) {
            return queryHandler_(sql, result);
        }
        result = lastQueryResult_;
        return shouldSucceed_;
    }

    void disconnect() override {
        disconnectCalled_ = true;
    }

    bool isConnected() const override {
        return connected_;
    }

    void setShouldSucceed(bool succeed) { shouldSucceed_ = succeed; }
    void setConnected(bool c) { connected_ = c; }
    void setLastQueryResult(const std::string& r) { lastQueryResult_ = r; }

    void setQueryHandler(std::function<bool(const std::string&, std::string&)> handler) {
        queryHandler_ = std::move(handler);
    }

    const std::vector<std::string>& connectCalls() const { return connectCalls_; }
    const std::vector<std::string>& queryCalls() const { return queryCalls_; }
    bool disconnectCalled() const { return disconnectCalled_; }
    std::size_t connectCallCount() const { return connectCalls_.size(); }
    std::size_t queryCallCount() const { return queryCalls_.size(); }

private:
    bool shouldSucceed_ = true;
    bool connected_ = false;
    bool disconnectCalled_ = false;
    std::string lastQueryResult_;
    std::vector<std::string> connectCalls_;
    std::vector<std::string> queryCalls_;
    std::function<bool(const std::string&, std::string&)> queryHandler_;
};

class UserRepository {
public:
    explicit UserRepository(Database& db) : db_(db) {}

    bool initialize(const std::string& url) {
        return db_.connect(url);
    }

    bool findUser(const std::string& name, std::string& result) {
        return db_.query("SELECT * FROM users WHERE name='" + name + "'", result);
    }

    void shutdown() {
        db_.disconnect();
    }

private:
    Database& db_;
};

} // namespace mock_objects
