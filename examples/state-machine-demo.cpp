// examples/state-machine-demo.cpp
// Demonstrates State Machine Pattern: Order lifecycle with explicit transitions.
// Compile: g++ -std=c++20 -o state-machine-demo state-machine-demo.cpp

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace state_machine {

enum class OrderState { Created, Processing, Shipped, Delivered, Cancelled };
enum class OrderEvent { Ship, Deliver, Cancel };

class StateMachine {
public:
    using Action = std::function<void()>;

    void addTransition(OrderState from, OrderEvent event,
                       OrderState to, Action action = nullptr) {
        transitions_[{from, event}] = {to, std::move(action)};
    }

    void handleEvent(OrderEvent event) {
        auto it = transitions_.find({currentState_, event});
        if (it == transitions_.end()) {
            throw std::runtime_error("No transition for event in current state");
        }
        auto& [to, action] = it->second;
        if (action) action();
        currentState_ = to;
    }

    OrderState currentState() const { return currentState_; }

    void setState(OrderState state) { currentState_ = state; }

private:
    OrderState currentState_ = OrderState::Created;
    std::map<std::pair<OrderState, OrderEvent>, std::pair<OrderState, Action>> transitions_;
};

} // namespace state_machine

std::string toString(state_machine::OrderState s) {
    using OrderState = state_machine::OrderState;
    switch (s) {
        case OrderState::Created: return "Created";
        case OrderState::Processing: return "Processing";
        case OrderState::Shipped: return "Shipped";
        case OrderState::Delivered: return "Delivered";
        case OrderState::Cancelled: return "Cancelled";
        default: return "Unknown";
    }
}

int main() {
    using namespace state_machine;
    std::cout << "=== State Machine Demo ===\n\n";

    StateMachine order;

    order.addTransition(OrderState::Created, OrderEvent::Ship,
                        OrderState::Processing);
    order.addTransition(OrderState::Processing, OrderEvent::Ship,
                        OrderState::Shipped);
    order.addTransition(OrderState::Processing, OrderEvent::Cancel,
                        OrderState::Cancelled);
    order.addTransition(OrderState::Shipped, OrderEvent::Deliver,
                        OrderState::Delivered);

    std::cout << "Order state: " << toString(order.currentState()) << "\n";

    order.handleEvent(OrderEvent::Ship);
    std::cout << "After Ship: " << toString(order.currentState()) << "\n";

    order.handleEvent(OrderEvent::Ship);
    std::cout << "After Ship: " << toString(order.currentState()) << "\n";

    order.handleEvent(OrderEvent::Deliver);
    std::cout << "After Deliver: " << toString(order.currentState()) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
