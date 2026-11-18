#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>

namespace observer_mediator {

// --- Event ---

struct Event {
    std::string type;
    std::string data;
};

// --- Handler ---

using EventHandler = std::function<void(const Event&)>;

// --- Mediator ---

class Mediator {
public:
    void registerHandler(const std::string& eventType, EventHandler handler) {
        handlers_[eventType].push_back(std::move(handler));
    }

    void notify([[maybe_unused]] const std::string& sender, const Event& event) {
        auto it = handlers_.find(event.type);
        if (it == handlers_.end()) {
            return;
        }

        // A handler can register another handler for the same event while
        // this dispatch is running; copy first so the loop stays valid.
        std::vector<EventHandler> snapshot = it->second;
        for (auto& handler : snapshot) {
            handler(event);
        }
    }

private:
    std::map<std::string, std::vector<EventHandler>> handlers_;
};

} // namespace observer_mediator
