#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <memory>

namespace mediator_pattern {

// --- Mediator Interface ---

class Mediator {
public:
    virtual ~Mediator() = default;
    virtual void notify(const std::string& sender, const std::string& event) = 0;
};

// --- Colleague Base ---

class Colleague {
public:
    explicit Colleague(std::shared_ptr<Mediator> mediator, const std::string& name)
        : mediator_(mediator), name_(name) {}

    virtual ~Colleague() = default;

    std::string getName() const { return name_; }

protected:
    std::shared_ptr<Mediator> mediator_;
    std::string name_;
};

// --- Concrete Mediator ---

class ChatRoom : public Mediator {
public:
    void addColleague(std::shared_ptr<Colleague> colleague) {
        colleagues_[colleague->getName()] = colleague;
    }

    void notify(const std::string& sender, const std::string& event) override {
        for (auto& [name, colleague] : colleagues_) {
            if (name != sender) {
                // In real implementation, would call colleague's receive method
                std::cout << "[" << sender << " -> " << name << "] " << event << "\n";
            }
        }
    }

private:
    std::map<std::string, std::shared_ptr<Colleague>> colleagues_;
};

// --- Concrete Colleagues ---

class User : public Colleague {
public:
    User(std::shared_ptr<Mediator> mediator, const std::string& name)
        : Colleague(mediator, name) {}

    void send(const std::string& message) {
        mediator_->notify(name_, message);
    }

    void receive(const std::string& from, const std::string& message) {
        std::cout << "[Receive] " << from << ": " << message << "\n";
    }
};

} // namespace mediator_pattern
