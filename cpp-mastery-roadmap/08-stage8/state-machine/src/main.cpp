#include "state_machine.hpp"

#include <iostream>
#include <string>

enum class OrderState { Created, Processing, Shipped, Delivered, Cancelled };
enum class OrderEvent { Ship, Deliver, Cancel };

int main() {
    std::cout << "=== State Machine Demo ===\n\n";

    state_machine::StateMachine<OrderState, OrderEvent> order;

    order.setState(OrderState::Created);
    order.addTransition(OrderState::Created, OrderEvent::Ship, OrderState::Processing);
    order.addTransition(OrderState::Processing, OrderEvent::Ship, OrderState::Shipped);
    order.addTransition(OrderState::Processing, OrderEvent::Cancel, OrderState::Cancelled);
    order.addTransition(OrderState::Shipped, OrderEvent::Deliver, OrderState::Delivered);

    std::cout << "Order state: Created\n";

    order.handleEvent(OrderEvent::Ship);
    std::cout << "After Ship: Processing\n";

    order.handleEvent(OrderEvent::Ship);
    std::cout << "After Ship: Shipped\n";

    order.handleEvent(OrderEvent::Deliver);
    std::cout << "After Deliver: Delivered\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
