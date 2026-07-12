#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>

namespace observer_event {

// --- Event ---

struct Event {
    std::string type;
    std::string source;
    std::string data;
};

// --- Handler ---

using EventHandler = std::function<void(const Event&)>;

// --- Event Dispatcher ---

class EventDispatcher {
public:
    void on(const std::string& eventType, EventHandler handler) {
        handlers_[eventType].push_back(std::move(handler));
    }

    void emit(const Event& event) {
        auto it = handlers_.find(event.type);
        if (it != handlers_.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

    void off(const std::string& eventType) {
        handlers_.erase(eventType);
    }

    std::size_t handlerCount(const std::string& eventType) const {
        auto it = handlers_.find(eventType);
        return (it != handlers_.end()) ? it->second.size() : 0;
    }

private:
    std::map<std::string, std::vector<EventHandler>> handlers_;
};

} // namespace observer_event
