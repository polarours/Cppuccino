#pragma once

#include <string>
#include <vector>
#include <functional>
#include <algorithm>

namespace observer_filter {

// --- Event ---

struct Event {
    std::string type;
    std::string data;
    int priority;
};

// --- Filter ---

using EventFilter = std::function<bool(const Event&)>;

// --- Filtered Dispatcher ---

class FilteredDispatcher {
public:
    void addFilter(EventFilter filter) {
        filters_.push_back(std::move(filter));
    }

    void onEvent(std::function<void(const Event&)> handler) {
        handler_ = handler;
    }

    void emit(const Event& event) {
        for (const auto& filter : filters_) {
            if (!filter(event)) {
                return;  // Filtered out
            }
        }
        if (handler_) {
            handler_(event);
        }
    }

private:
    std::vector<EventFilter> filters_;
    std::function<void(const Event&)> handler_;
};

} // namespace observer_filter
