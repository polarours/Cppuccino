// examples/cqrs-pattern-demo.cpp
// Demonstrates CQRS Pattern: CommandBus/QueryBus separated, AccountService with in-memory store.
// Compile: g++ -std=c++20 -o cqrs-pattern-demo cqrs-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace cqrs_pattern {

struct Account {
    int id;
    std::string name;
    double balance;
};

class CommandBus {
public:
    using Handler = std::function<void(const std::string&)>;
    void registerHandler(const std::string& type, Handler h) { handlers_[type] = std::move(h); }
    void dispatch(const std::string& type, const std::string& payload) {
        if (handlers_.count(type)) handlers_[type](payload);
    }
private:
    std::map<std::string, Handler> handlers_;
};

class QueryBus {
public:
    using Handler = std::function<std::string(const std::string&)>;
    void registerHandler(const std::string& type, Handler h) { handlers_[type] = std::move(h); }
    std::string query(const std::string& type, const std::string& params) {
        if (handlers_.count(type)) return handlers_[type](params);
        return "";
    }
private:
    std::map<std::string, Handler> handlers_;
};

class AccountService {
public:
    void createAccount(const std::string& name) {
        accounts_[nextId_++] = {nextId_, name, 0.0};
    }
    void deposit(int id, double amount) {
        if (accounts_.count(id)) accounts_[id].balance += amount;
    }
    std::shared_ptr<Account> getAccount(int id) {
        if (!accounts_.count(id)) return nullptr;
        return std::make_shared<Account>(accounts_[id]);
    }
    AccountService() {
        commands_.registerHandler("create", [this](const std::string& name){ createAccount(name); });
        commands_.registerHandler("deposit", [this](const std::string& p){
            auto sep = p.find(':');
            deposit(std::stoi(p.substr(0,sep)), std::stod(p.substr(sep+1)));
        });
        queries_.registerHandler("get", [this](const std::string& id){
            auto acc = getAccount(std::stoi(id));
            return acc ? acc->name + ":" + std::to_string(acc->balance) : "";
        });
        queries_.registerHandler("list", [this](const std::string&){
            std::string r;
            for (const auto& [_, a] : accounts_)
                r += (r.empty()?"":",") + a.name;
            return r;
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

int main() {
    using namespace cqrs_pattern;
    std::cout << "=== CQRS Pattern Demo ===\n\n";

    AccountService service;

    // Commands
    service.commands().dispatch("create", "Alice");
    service.commands().dispatch("deposit", "1:1000.0");
    service.commands().dispatch("deposit", "1:500.0");

    // Queries
    std::cout << "Get account: " << service.queries().query("get", "1") << "\n";
    std::cout << "All accounts: " << service.queries().query("list", "") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
