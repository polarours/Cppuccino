// examples/chain-of-responsibility-demo.cpp
// Demonstrates Chain of Responsibility: Auth -> Logging -> Validation -> Processing.
// Compile with: g++ -std=c++20 -o chain-of-responsibility-demo chain-of-responsibility-demo.cpp

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
            std::cout << "[Auth] rejected: empty request\n";
            return false;
        }
        std::cout << "[Auth] passed\n";
        return next_ ? next_->handle(req) : true;
    }
};

class LoggingHandler : public Handler {
public:
    bool handle(Request& req) override {
        std::cout << "[Log] recording request: " << req.content << " (priority:" << req.priority << ")\n";
        return next_ ? next_->handle(req) : true;
    }
};

class ValidationHandler : public Handler {
public:
    bool handle(Request& req) override {
        if (req.content.size() > 100) {
            std::cout << "[Validation] rejected: content too long\n";
            return false;
        }
        std::cout << "[Validation] passed\n";
        return next_ ? next_->handle(req) : true;
    }
};

class ProcessingHandler : public Handler {
public:
    bool handle(Request& req) override {
        std::cout << "[Processing] handling: " << req.content << "\n";
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
