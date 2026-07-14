#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>
#include <iostream>

namespace observer_subscription {

struct Event {
    std::string type;
    std::string data;
};

using Handler = std::function<void(const Event&)>;

class Subscription {
public:
    Subscription(std::size_t id, std::function<void()> unsub)
        : id_(id), unsub_(std::move(unsub)) {}

    ~Subscription() { unsubscribe(); }

    void unsubscribe() {
        if (unsub_) {
            unsub_();
            unsub_ = nullptr;
        }
    }

    std::size_t id() const { return id_; }
    bool active() const { return unsub_ != nullptr; }

    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;

    Subscription(Subscription&& other) noexcept
        : id_(other.id_), unsub_(std::move(other.unsub_)) {
        other.unsub_ = nullptr;
    }

private:
    std::size_t id_;
    std::function<void()> unsub_;
};

class EventBus {
public:
    std::unique_ptr<Subscription> subscribe(const std::string& eventType, Handler handler) {
        std::size_t id = nextId_++;
        handlers_[eventType].push_back({id, std::move(handler)});

        return std::make_unique<Subscription>(id, [this, eventType, id]() {
            auto it = handlers_.find(eventType);
            if (it != handlers_.end()) {
                auto& vec = it->second;
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                    [id](const Entry& e) { return e.id == id; }), vec.end());
            }
        });
    }

    void publish(const Event& event) {
        auto it = handlers_.find(event.type);
        if (it != handlers_.end()) {
            for (const auto& entry : it->second) {
                entry.handler(event);
            }
        }
    }

    std::size_t handlerCount(const std::string& eventType) const {
        auto it = handlers_.find(eventType);
        return (it != handlers_.end()) ? it->second.size() : 0;
    }

private:
    struct Entry {
        std::size_t id;
        Handler handler;
    };

    std::map<std::string, std::vector<Entry>> handlers_;
    std::size_t nextId_ = 1;
};

} // namespace observer_subscription
