#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>

namespace mini_http {

struct Event {
    std::string type;
    std::string data;
};

using EventHandler = std::function<void(const Event&)>;

class EventBus {
public:
    void subscribe(const std::string& eventType, EventHandler handler) {
        handlers_[eventType].push_back(std::move(handler));
    }

    void publish(const Event& event) {
        auto it = handlers_.find(event.type);
        if (it != handlers_.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

    std::size_t handlerCount(const std::string& eventType) const {
        auto it = handlers_.find(eventType);
        return (it != handlers_.end()) ? it->second.size() : 0;
    }

    void clear() { handlers_.clear(); }

private:
    std::map<std::string, std::vector<EventHandler>> handlers_;
};

} // namespace mini_http
