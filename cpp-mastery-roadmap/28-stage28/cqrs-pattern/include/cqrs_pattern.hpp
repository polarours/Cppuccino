#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

namespace cqrs_pattern {

struct Account {
    int id;
    std::string name;
    double balance;
};

class CommandBus {
public:
    using Handler = std::function<void(const std::string&)>;

    void registerHandler(const std::string& commandType, Handler handler) {
        handlers_[commandType] = std::move(handler);
    }

    void dispatch(const std::string& commandType, const std::string& payload) {
        auto it = handlers_.find(commandType);
        if (it != handlers_.end()) {
            it->second(payload);
        }
    }

private:
    std::map<std::string, Handler> handlers_;
};

class QueryBus {
public:
    using Handler = std::function<std::string(const std::string&)>;

    void registerHandler(const std::string& queryType, Handler handler) {
        handlers_[queryType] = std::move(handler);
    }

    std::string query(const std::string& queryType, const std::string& params) {
        auto it = handlers_.find(queryType);
        if (it != handlers_.end()) {
            return it->second(params);
        }
        return "";
    }

private:
    std::map<std::string, Handler> handlers_;
};

class AccountService {
public:
    void createAccount(const std::string& name) {
        accounts_[nextId_] = {nextId_, name, 0.0};
        nextId_++;
    }

    void deposit(int id, double amount) {
        auto it = accounts_.find(id);
        if (it != accounts_.end()) {
            it->second.balance += amount;
        }
    }

    std::shared_ptr<Account> getAccount(int id) {
        auto it = accounts_.find(id);
        return (it != accounts_.end()) ? std::make_shared<Account>(it->second) : nullptr;
    }

    AccountService() {
        commands_.registerHandler("create", [this](const std::string& payload) {
            createAccount(payload);
        });
        commands_.registerHandler("deposit", [this](const std::string& payload) {
            auto pos = payload.find(':');
            int id = std::stoi(payload.substr(0, pos));
            double amount = std::stod(payload.substr(pos + 1));
            deposit(id, amount);
        });

        queries_.registerHandler("get", [this](const std::string& params) {
            int id = std::stoi(params);
            auto acc = getAccount(id);
            if (acc) return acc->name + ":" + std::to_string(acc->balance);
            return std::string();
        });
        queries_.registerHandler("list", [this](const std::string&) {
            std::string result;
            for (const auto& [id, acc] : accounts_) {
                if (!result.empty()) result += ",";
                result += acc.name;
            }
            return result;
        });
    }

    CommandBus& commands() { return commands_; }
    QueryBus& queries() { return queries_; }

private:
    std::map<int, Account> accounts_;
    int nextId_ = 1;
    CommandBus commands_;
    QueryBus queries_;
};

} // namespace cqrs_pattern
