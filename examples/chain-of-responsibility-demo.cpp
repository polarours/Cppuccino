// examples/chain-of-responsibility-demo.cpp
// Demonstrates Chain of Responsibility: Auth -> Logging -> Validation -> Processing.
// Compile: g++ -std=c++20 -o chain-of-responsibility-demo chain-of-responsibility-demo.cpp

#include <iostream>
#include <memory>
#include <string>

namespace chain_of_responsibility {

struct Request {
    std::string content;
    int priority;
};

class Handler {
public:
    virtual ~Handler() = default;
    virtual bool handle(Request& req) = 0;
    void setNext(std::shared_ptr<Handler> next) { next_ = std::move(next); }
protected:
    std::shared_ptr<Handler> next_;
};

class AuthHandler : public Handler {
public:
    bool handle(Request& req) override {
        if (req.content.empty()) {
            std::cout << "[Auth] 拒绝：空请求\n";
            return false;
        }
        std::cout << "[Auth] 通过\n";
        return next_ ? next_->handle(req) : true;
    }
};

class LoggingHandler : public Handler {
public:
    bool handle(Request& req) override {
        std::cout << "[Log] 记录请求: " << req.content << " (优先级:" << req.priority << ")\n";
        return next_ ? next_->handle(req) : true;
    }
};

class ValidationHandler : public Handler {
public:
    bool handle(Request& req) override {
        if (req.content.size() > 100) {
            std::cout << "[Validation] 拒绝：内容过长\n";
            return false;
        }
        std::cout << "[Validation] 通过\n";
        return next_ ? next_->handle(req) : true;
    }
};

class ProcessingHandler : public Handler {
public:
    bool handle(Request& req) override {
        std::cout << "[Processing] 处理: " << req.content << "\n";
        return true;
    }
};

} // namespace chain_of_responsibility

int main() {
    using namespace chain_of_responsibility;
    std::cout << "=== Chain of Responsibility Demo ===\n\n";

    auto auth = std::make_shared<AuthHandler>();
    auto logging = std::make_shared<LoggingHandler>();
    auto validation = std::make_shared<ValidationHandler>();
    auto processing = std::make_shared<ProcessingHandler>();

    auth->setNext(logging);
    logging->setNext(validation);
    validation->setNext(processing);

    Request req{"Hello World", 1};
    auth->handle(req);

    std::cout << "\n--- Empty request ---\n";
    Request badReq{"", 0};
    auth->handle(badReq);

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
