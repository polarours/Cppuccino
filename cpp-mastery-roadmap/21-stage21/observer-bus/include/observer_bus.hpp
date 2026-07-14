#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>

namespace observer_bus {

struct Event {
    std::string type;
    std::string data;
};

using Handler = std::function<void(const Event&)>;

class EventBus {
public:
    void subscribe(const std::string& event, Handler handler) {
        handlers_[event].push_back(std::move(handler));
    }

    void publish(const Event& event) {
        auto it = handlers_.find(event.type);
        if (it != handlers_.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

    std::size_t handlerCount(const std::string& event) const {
        auto it = handlers_.find(event);
        return (it != handlers_.end()) ? it->second.size() : 0;
    }

private:
    std::map<std::string, std::vector<Handler>> handlers_;
};

} // namespace observer_bus
