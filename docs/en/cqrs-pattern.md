# CQRS 模式 (CQRS Pattern)

## 什么是 CQRS 模式？

CQRS（Command Query Responsibility Segregation）模式将读操作和写操作分离到不同的模型中。

> **核心思想**：命令（写）和查询（读）使用不同的模型，可以提高性能和可扩展性。

## 模式组成

- **Command（命令）**：改变状态的操作
- **Query（查询）**：读取数据不改变状态
- **Command Handler**：处理写操作
- **Query Handler**：处理读操作

## 代码实现

```cpp
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>

// 命令
class Command {
public:
    virtual ~Command() = default;
};

class CreateUserCommand : public Command {
public:
    std::string name;
    std::string email;
};

// 查询
class Query {
public:
    virtual ~Query() = default;
};

class GetUserQuery : public Query {
public:
    int id;
};

// 结果
class Result {
public:
    virtual ~Result() = default;
};

class UserResult : public Result {
public:
    int id;
    std::string name;
    std::string email;
};

// 命令处理器
class CommandHandler {
public:
    virtual ~CommandHandler() = default;
    virtual void handle(const Command& cmd) = 0;
};

class CreateUserCommandHandler : public CommandHandler {
public:
    void handle(const Command& cmd) override {
        const auto& createCmd = static_cast<const CreateUserCommand&>(cmd);
        // 写入命令存储
        commands_.push_back(createCmd);
        std::cout << "User created: " << createCmd.name << "\n";
    }
    
    const std::vector<CreateUserCommand>& getCommands() const { return commands_; }

private:
    std::vector<CreateUserCommand> commands_;
};

// 查询处理器
class QueryHandler {
public:
    virtual ~QueryHandler() = default;
    virtual std::unique_ptr<Result> handle(const Query& query) = 0;
};

class GetUserQueryHandler : public QueryHandler {
public:
    void setUsers(const std::vector<UserResult>& users) { users_ = users; }
    
    std::unique_ptr<Result> handle(const Query& query) override {
        const auto& getUserQuery = static_cast<const GetUserQuery&>(query);
        for (const auto& user : users_) {
            if (user.id == getUserQuery.id) {
                return std::make_unique<UserResult>(user);
            }
        }
        return nullptr;
    }

private:
    std::vector<UserResult> users_;
};

// 指令/查询分离
class CQRSBus {
public:
    template<typename Command>
    void send(const Command& cmd) {
        auto handler = commandHandlers_.find(typeid(Command).name());
        if (handler != commandHandlers_.end()) {
            handler->second->handle(cmd);
        }
    }
    
    template<typename Query, typename Result>
    std::unique_ptr<Result> query(const Query& q) {
        auto handler = queryHandlers_.find(typeid(Query).name());
        if (handler != queryHandlers_.end()) {
            return std::dynamic_pointer_cast<Result>(handler->second->handle(q));
        }
        return nullptr;
    }
    
    template<typename C, typename H>
    void registerCommandHandler() {
        commandHandlers_[typeid(C).name()] = std::make_shared<H>();
    }
    
    template<typename Q, typename H>
    void registerQueryHandler() {
        queryHandlers_[typeid(Q).name()] = std::make_shared<H>();
    }

private:
    std::map<std::string, std::shared_ptr<CommandHandler>> commandHandlers_;
    std::map<std::string, std::shared_ptr<QueryHandler>> queryHandlers_;
};
```

## 最佳实践

1. 读写模型分离
2. 使用事件溯源配合
3. 异步处理命令

## 总结

CQRS 通过分离读写操作，提供更高的性能和可扩展性。
