# Chain of Responsibility Pattern

## What Is the Chain of Responsibility Pattern?

Chain of Responsibility is a behavioral design pattern that links all request
handlers into a chain and passes the request along it until some object
handles it.

> **Core idea**: decouple the sender of a request from its receivers — let
> several objects each get a chance to handle it.

## Why Do We Need It?

### Problem Scenario

Consider a permission-checking system:

```cpp
#include <string>

// stand-in helpers for the checks
bool isAuthenticated(const std::string&) { return true; }
bool hasRole(const std::string&, const std::string&) { return true; }
bool hasPermission(const std::string&, const std::string&) { return true; }
bool isLocked(const std::string&) { return false; }

// Problem: a tangle of conditionals
bool checkPermission(const std::string& user, const std::string& action) {
    if (!isAuthenticated(user)) {
        return false;  // not authenticated
    }
    if (!hasRole(user, "admin") && action == "delete") {
        return false;  // no delete permission
    }
    if (!hasPermission(user, action)) {
        return false;  // no permission for this action
    }
    if (isLocked(user)) {
        return false;  // account locked
    }
    // ... more checks
    return true;
}
```

**The problems**:

1. The conditionals are hard to read
2. Adding a rule means editing the function
3. Individual rules cannot be tested in isolation

### Solution

Use the chain of responsibility:

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

**Benefits**:

1. Decoupling: the sender never knows the handlers
2. Extensible: new handlers plug in without touching existing ones
3. Flexible: the order can change at runtime

## Pattern Structure

```
                    ┌─────────────┐
                    │  Handler    │
                    │  (处理器)    │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  AuthHandler│ │ RoleHandler│ │PermissionH.│ ← concrete handlers
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← the client
                    └─────────────────┘
```

## Implementation

### 1. Define the Handler Interface

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

### 2. Implement the Concrete Handlers

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

### 3. Build the Chain

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

### 4. Client Usage

```cpp
int main() {
    RequestPipeline pipeline;

    // build the chain
    pipeline.addHandler(std::make_unique<AuthHandler>());
    pipeline.addHandler(std::make_unique<PermissionHandler>());
    pipeline.addHandler(std::make_unique<RateLimitHandler>());
    pipeline.addHandler(std::make_unique<LoggingHandler>());

    // handle requests
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

## Modern C++ Implementations

### With Lambdas

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

### With a Variadic Constructor

```cpp
#include <functional>
#include <string>
#include <vector>
#include <utility>

// a variadic constructor folds any number of callables into one chain
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
                return false;   // short-circuit
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

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Strategy** | Strategy picks one algorithm; a chain passes the request to many handlers |
| **Command** | Command encapsulates a request; a chain processes a sequence of handlers |
| **Observer** | Observer notifies many objects; a chain processes them in order, stopping at the first that handles it |

## Best Practices

### 1. Build Chains with a Factory

```cpp
class HandlerFactory {
public:
    static std::unique_ptr<LogHandler> createChain() {
        // build tail-first: a handler must not be used after it was moved
        auto rateLimit = std::make_unique<RateLimitHandler>();
        auto permission = std::make_unique<PermissionHandler>();
        auto auth = std::make_unique<AuthHandler>();

        permission->setNext(std::move(rateLimit));
        auth->setNext(std::move(permission));

        return auth;
    }
};
```

### 2. Support Short-Circuiting

```cpp
class ShortCircuitHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        bool result = handleRequest(request);
        if (!result) {
            return false;  // short-circuit: do not forward to the next handler
        }
        return LogHandler::handle(request);
    }

private:
    virtual bool handleRequest(const std::string& request) = 0;
};
```

### 3. Use the Middleware Shape

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

## Common Traps

### Trap 1: The Chain Gets Too Long

```cpp
// Not recommended: a 10+ handler chain is slow and unreadable
// handler1->setNext(std::make_unique<Handler2>());
// handler2->setNext(std::make_unique<Handler3>());
// ...
//
// Recommended: keep chains short and split them
// class AuthChain { /* ... */ };       // authentication chain
// class PermissionChain { /* ... */ }; // permission chain
```

### Trap 2: Cycles

```cpp
// Danger: the chain forms a cycle
// handlerA->setNext(std::move(handlerB));
// handlerB->setNext(std::move(handlerC));
// handlerC->setNext(std::move(handlerA));  // cycle!
//
// (with unique_ptr the last line would not even compile — which is your
//  friend; if you must reference backwards, use a non-owning pointer and
//  keep the chain strictly linear)
```

### Trap 3: Forgetting to Terminate the Chain

```cpp
#include <memory>
#include <string>

class LogHandler {  // the base from section 1
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

// Not recommended: never forwards — the chain silently ends here
class BadHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        // never calls next_->handle()
        return false;
    }
};

// Recommended: forward correctly
class GoodHandler : public LogHandler {
public:
    bool handle(const std::string& request) override {
        bool result = process(request);
        if (!result) return false;
        return LogHandler::handle(request);  // pass to the next handler
    }

private:
    bool process(const std::string& request) { return true; }  // stand-in logic
};
```

## Example

A runnable version of this topic:

- [chain-of-responsibility-demo.cpp](../../examples/chain-of-responsibility-demo.cpp)

## Summary

The core points of the chain of responsibility pattern:

- **When to use it**: many objects may handle a request, the handler should be
  chosen at runtime, or deep conditional nesting must go
- **Main benefits**: decoupling, extensibility, flexibility
- **Implementation advice**: build chains with a factory, support
  short-circuiting, make sure every chain terminates
- **Common traps**: chains that are too long, cycles, forgetting to terminate

> **Remember**: the key is *ordered processing* — the request travels along
> the chain, and every handler gets a chance to handle or reject it.
