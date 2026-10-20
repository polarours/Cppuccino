# 责任链模式

## 什么是责任链模式？

责任链模式（Chain of Responsibility Pattern）是一种行为型设计模式，它将所有请求的处理者连成一条链，并沿着这条链传递请求，直到有对象处理它为止。

> **核心思想**：将请求的发送者和接收者解耦，让多个对象都有机会处理请求。

## 为什么需要责任链模式？

### 问题场景

假设我们有一个权限验证系统：

```cpp
#include <string>

// 检查所依赖的辅助函数（占位实现）
bool isAuthenticated(const std::string&) { return true; }
bool hasRole(const std::string&, const std::string&) { return true; }
bool hasPermission(const std::string&, const std::string&) { return true; }
bool isLocked(const std::string&) { return false; }

// 问题：复杂的条件判断
bool checkPermission(const std::string& user, const std::string& action) {
    if (!isAuthenticated(user)) {
        return false;  // 未认证
    }
    if (!hasRole(user, "admin") && action == "delete") {
        return false;  // 无删除权限
    }
    if (!hasPermission(user, action)) {
        return false;  // 无操作权限
    }
    if (isLocked(user)) {
        return false;  // 账户锁定
    }
    // ... 更多检查
    return true;
}
```

**问题**：

1. 条件判断复杂
2. 添加新的验证规则需要修改函数
3. 难以测试单个验证规则

### 解决方案

使用责任链模式：

```cpp
#include <memory>
#include <string>

bool isAuthenticated(const std::string&) { return true; }
bool hasRole(const std::string&, const std::string&) { return true; }

class Handler {
public:
    virtual ~Handler() = default;
    void setNext(std::unique_ptr<Handler> next) { next_ = std::move(next); }
    virtual bool handle(const std::string& user, const std::string& action) {
        if (next_) {
            return next_->handle(user, action);
        }
        return true;
    }

protected:
    std::unique_ptr<Handler> next_;
};

class AuthHandler : public Handler {
public:
    bool handle(const std::string& user, const std::string& action) override {
        if (!isAuthenticated(user)) {
            return false;
        }
        return Handler::handle(user, action);
    }
};

class RoleHandler : public Handler {
public:
    bool handle(const std::string& user, const std::string& action) override {
        if (action == "delete" && !hasRole(user, "admin")) {
            return false;
        }
        return Handler::handle(user, action);
    }
};
```

**优点**：

1. 解耦：请求发送者不需要知道处理者
2. 可扩展：可以轻松添加新的处理者
3. 灵活：可以动态改变链的顺序

## 模式结构

```
                    ┌─────────────┐
                    │  Handler    │
                    │  (处理器)    │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  AuthHandler│ │ RoleHandler│ │PermissionH.│ ← 具体处理器
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← 客户端
                    └─────────────────┘
```

## 代码实现

### 1. 定义处理器接口

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <functional>

class LogHandler {
public:
    virtual ~LogHandler() = default;
    void setNext(std::unique_ptr<LogHandler> handler) {
        next_ = std::move(handler);
    }

    virtual bool handle(const std::string& request) {
        if (next_) {
            return next_->handle(request);
        }
        return true;
    }

    LogHandler* next() const { return next_.get(); }  // public view of the rest of the chain

protected:
    std::unique_ptr<LogHandler> next_;
};
```

### 2. 实现具体处理器

```cpp
class AuthHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        std::cout << "[Auth] Checking authentication for: " << request << "\n";
        if (request.find("unauthorized") != std::string::npos) {
            std::cout << "[Auth] Access denied!\n";
            return false;
        }
        return LogHandler::handle(request);
    }
};

class PermissionHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        std::cout << "[Permission] Checking permissions for: " << request << "\n";
        if (request.find("forbidden") != std::string::npos) {
            std::cout << "[Permission] Permission denied!\n";
            return false;
        }
        return LogHandler::handle(request);
    }
};

class RateLimitHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        std::cout << "[RateLimit] Checking rate limit for: " << request << "\n";
        if (request.find("spam") != std::string::npos) {
            std::cout << "[RateLimit] Rate limit exceeded!\n";
            return false;
        }
        return LogHandler::handle(request);
    }
};

class LoggingHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        std::cout << "[Logging] Logging request: " << request << "\n";
        return LogHandler::handle(request);
    }
};
```

### 3. 构建责任链

```cpp
class RequestPipeline {
public:
    void addHandler(std::unique_ptr<LogHandler> handler) {
        if (!head_) {
            head_ = std::move(handler);
        } else {
            tail_->setNext(std::move(handler));
        }
        tail_ = head_.get();
        while (tail_->next()) {
            tail_ = tail_->next();
        }
    }

    bool process(const std::string& request) {
        std::cout << "Processing: " << request << "\n";
        bool result = head_ ? head_->handle(request) : true;
        std::cout << "Result: " << (result ? "ALLOWED" : "DENIED") << "\n";
        return result;
    }

private:
    std::unique_ptr<LogHandler> head_;
    LogHandler* tail_ = nullptr;
};
```

### 4. 客户端使用

```cpp
int main() {
    RequestPipeline pipeline;

    // 构建责任链
    pipeline.addHandler(std::make_unique<AuthHandler>());
    pipeline.addHandler(std::make_unique<PermissionHandler>());
    pipeline.addHandler(std::make_unique<RateLimitHandler>());
    pipeline.addHandler(std::make_unique<LoggingHandler>());

    // 处理请求
    pipeline.process("normal_request");
    std::cout << "-------------------\n";
    pipeline.process("unauthorized_request");
    std::cout << "-------------------\n";
    pipeline.process("forbidden_request");
    std::cout << "-------------------\n";
    pipeline.process("spam_request");

    return 0;
}
```

## 现代 C++ 实现

### 使用 lambda 简化

```cpp
#include <memory>
#include <string>
#include <functional>
#include <iostream>

class LambdaHandler {
public:
    using HandlerFunc = std::function<bool(const std::string&)>;

    LambdaHandler(HandlerFunc func, std::unique_ptr<LambdaHandler> next = nullptr)
        : func_(std::move(func)), next_(std::move(next)) {}

    bool handle(const std::string& request) {
        if (!func_(request)) {
            return false;
        }
        return next_ ? next_->handle(request) : true;
    }

private:
    HandlerFunc func_;
    std::unique_ptr<LambdaHandler> next_;
};

void demoLambda() {
    auto handler = std::make_unique<LambdaHandler>(
        [](const std::string& req) {
            std::cout << "[Auth] Checking...\n";
            return req.find("unauthorized") == std::string::npos;
        },
        std::make_unique<LambdaHandler>(
            [](const std::string& req) {
                std::cout << "[Permission] Checking...\n";
                return req.find("forbidden") == std::string::npos;
            }
        )
    );
    (void)handler;
}
```

### 使用 variadic constructor

```cpp
#include <functional>
#include <string>
#include <vector>
#include <utility>

// 可变参数构造函数：把任意数量的 callable 折叠成一条链
class Chain {
public:
    using HandlerFunc = std::function<bool(const std::string&)>;

    template <typename... Fs>
    explicit Chain(Fs&&... fs) {
        (steps_.push_back(HandlerFunc(std::forward<Fs>(fs))), ...);
    }

    bool handle(const std::string& request) {
        for (auto& step : steps_) {
            if (!step(request)) {
                return false;   // 短路
            }
        }
        return true;
    }

private:
    std::vector<HandlerFunc> steps_;
};

void demoVariadic() {
    Chain chain(
        [](const std::string& r) { return r.find("unauthorized") == std::string::npos; },
        [](const std::string& r) { return r.find("forbidden") == std::string::npos; }
    );
    (void)chain.handle("ok_request");
}
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **策略模式** | 策略模式选择一个算法；责任链传递请求给多个处理者 |
| **命令模式** | 命令模式封装请求；责任链处理请求链 |
| **观察者模式** | 观察者模式通知多个对象；责任链按顺序处理 |

## 最佳实践

### 1. 使用工厂构建链

```cpp
class HandlerFactory {
public:
    static std::unique_ptr<LogHandler> createChain() {
        // 从尾部开始构建：handler 被 move 之后不能再使用
        auto rateLimit = std::make_unique<RateLimitHandler>();
        auto permission = std::make_unique<PermissionHandler>();
        auto auth = std::make_unique<AuthHandler>();

        permission->setNext(std::move(rateLimit));
        auth->setNext(std::move(permission));

        return auth;
    }
};
```

### 2. 支持短路处理

```cpp
class ShortCircuitHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        bool result = handleRequest(request);
        if (!result) {
            return false;  // 短路：不再传递给下一个
        }
        return LogHandler::handle(request);
    }

private:
    virtual bool handleRequest(const std::string& request) = 0;
};
```

### 3. 使用中间件模式

```cpp
#include <functional>
#include <string>
#include <iostream>

class Middleware {
public:
    using MiddlewareFunc = std::function<bool(const std::string&, std::function<bool()>)>;

    static MiddlewareFunc logging() {
        return [](const std::string& request, std::function<bool()> next) {
            std::cout << "[Logging] Before: " << request << "\n";
            bool result = next();
            std::cout << "[Logging] After: " << result << "\n";
            return result;
        };
    }

    static MiddlewareFunc auth() {
        return [](const std::string& request, std::function<bool()> next) {
            if (request.find("auth") != std::string::npos) {
                return false;
            }
            return next();
        };
    }
};
```

## 常见陷阱

### 陷阱 1：链过长

```cpp
// 不推荐：10+ 个处理器的链，既慢又难读
// handler1->setNext(std::make_unique<Handler2>());
// handler2->setNext(std::make_unique<Handler3>());
// ...
//
// 推荐：保持链短，必要时拆分
// class AuthChain { /* ... */ };       // 认证链
// class PermissionChain { /* ... */ }; // 权限链
```

### 陷阱 2：循环引用

```cpp
// 危险：链形成环
// handlerA->setNext(std::move(handlerB));
// handlerB->setNext(std::move(handlerC));
// handlerC->setNext(std::move(handlerA));  // 循环！
//
// （最后这行用 unique_ptr 根本编译不过——这其实是好事；如果确实需要
//  反向引用，用非拥有指针，并保持链严格线性）
```

### 陷阱 3：忘记终止链

```cpp
#include <memory>
#include <string>

class LogHandler {  // 第 1 节的基类
public:
    virtual ~LogHandler() = default;
    void setNext(std::unique_ptr<LogHandler> handler) { next_ = std::move(handler); }
    virtual bool handle(const std::string& request) {
        if (next_) return next_->handle(request);
        return true;
    }
protected:
    std::unique_ptr<LogHandler> next_;
};

// 不推荐：不转发——链在这里无声地结束了
class BadHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        // 忘记调用 next_->handle()
        return false;
    }
};

// 推荐：正确终止
class GoodHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        bool result = process(request);
        if (!result) return false;
        return LogHandler::handle(request);  // 传递给下一个
    }

private:
    bool process(const std::string& request) { return true; }  // 占位逻辑
};
```

## 总结

责任链模式的核心要点：

- **适用场景**：多个对象处理请求、动态指定处理者、简化条件分支
- **核心优点**：解耦、可扩展、灵活
- **实现要点**：使用工厂构建链、支持短路、确保链正确终止
- **常见陷阱**：链过长、循环引用、忘记终止

> **记住**：责任链模式的关键是"顺序处理"——让请求沿着处理者链条传递，每个处理者都有机会处理或拒绝请求。
