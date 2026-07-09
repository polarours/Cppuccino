#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <stdexcept>

namespace state_machine {

template <typename State, typename Event>
class StateMachine {
public:
    using Transition = std::function<void()>;

    void addTransition(State from, Event event, State to, Transition action = nullptr) {
        transitions_[{from, event}] = {to, std::move(action)};
    }

    void handleEvent(Event event) {
        auto it = transitions_.find({currentState_, event});
        if (it == transitions_.end()) {
            throw std::runtime_error("No transition for event in current state");
        }

        auto& [to, action] = it->second;
        if (action) {
            action();
        }
        currentState_ = to;
    }

    State currentState() const {
        return currentState_;
    }

    void setState(State state) {
        currentState_ = state;
    }

    bool canHandle(Event event) const {
        return transitions_.find({currentState_, event}) != transitions_.end();
    }

    std::vector<Event> availableEvents() const {
        std::vector<Event> events;
        for (const auto& pair : transitions_) {
            if (pair.first.from == currentState_) {
                events.push_back(pair.first.event);
            }
        }
        return events;
    }

private:
    struct TransitionKey {
        State from;
        Event event;

        bool operator<(const TransitionKey& other) const {
            if (from < other.from) return true;
            if (from > other.from) return false;
            return event < other.event;
        }
    };

    struct TransitionValue {
        State to;
        Transition action;
    };

    State currentState_;
    std::map<TransitionKey, TransitionValue> transitions_;
};

} // namespace state_machine
