#pragma once

#include <string>
#include <memory>
#include <iostream>

namespace chain_of_responsibility {

// --- Request ---

struct Request {
    std::string content;
    int priority;
};

// --- Handler Interface ---

class Handler {
public:
    virtual ~Handler() = default;

    void setNext(std::shared_ptr<Handler> next) {
        next_ = next;
    }

    virtual void handle(const Request& request) = 0;

protected:
    std::shared_ptr<Handler> next_;

    void passToNext(const Request& request) {
        if (next_) {
            next_->handle(request);
        }
    }
};

// --- Concrete Handlers ---

class AuthHandler : public Handler {
public:
    void handle(const Request& request) override {
        std::cout << "[Auth] Checking authentication for: " << request.content << "\n";
        passToNext(request);
    }
};

class LoggingHandler : public Handler {
public:
    void handle(const Request& request) override {
        std::cout << "[Log] Logging request: " << request.content << "\n";
        passToNext(request);
    }
};

class ValidationHandler : public Handler {
public:
    void handle(const Request& request) override {
        std::cout << "[Validation] Validating: " << request.content << "\n";
        if (request.content.empty()) {
            std::cout << "[Validation] Request rejected: empty content\n";
            return;
        }
        passToNext(request);
    }
};

class ProcessingHandler : public Handler {
public:
    void handle(const Request& request) override {
        std::cout << "[Processing] Processing: " << request.content << "\n";
    }
};

} // namespace chain_of_responsibility
