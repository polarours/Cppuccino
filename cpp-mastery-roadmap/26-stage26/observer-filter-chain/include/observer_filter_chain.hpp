#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace observer_filter_chain {

struct Event {
    std::string type;
    std::string data;
};

using Filter = std::function<Event(const Event&)>;

class FilterChain {
public:
    void addFilter(Filter filter) {
        filters_.push_back(std::move(filter));
    }

    Event apply(const Event& event) const {
        Event result = event;
        for (const auto& filter : filters_) {
            result = filter(result);
        }
        return result;
    }

    std::size_t size() const { return filters_.size(); }

private:
    std::vector<Filter> filters_;
};

class FilteredDispatcher {
public:
    explicit FilteredDispatcher(FilterChain chain) : chain_(std::move(chain)) {}

    void setHandler(std::function<void(const Event&)> handler) {
        handler_ = std::move(handler);
    }

    void emit(const Event& event) {
        auto filtered = chain_.apply(event);
        if (handler_) {
            handler_(filtered);
        }
    }

    FilterChain& chain() { return chain_; }

private:
    FilterChain chain_;
    std::function<void(const Event&)> handler_;
};

} // namespace observer_filter_chain
