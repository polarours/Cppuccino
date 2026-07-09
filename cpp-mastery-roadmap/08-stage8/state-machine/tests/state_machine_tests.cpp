#include "state_machine.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

enum class TrafficLight { Red, Yellow, Green };
enum class Signal { Timer };

void test_basic_transition() {
    state_machine::StateMachine<TrafficLight, Signal> sm;

    sm.setState(TrafficLight::Red);
    sm.addTransition(TrafficLight::Red, Signal::Timer, TrafficLight::Green);
    sm.addTransition(TrafficLight::Green, Signal::Timer, TrafficLight::Yellow);
    sm.addTransition(TrafficLight::Yellow, Signal::Timer, TrafficLight::Red);

    expect(sm.currentState() == TrafficLight::Red, "expected Red");

    sm.handleEvent(Signal::Timer);
    expect(sm.currentState() == TrafficLight::Green, "expected Green");

    sm.handleEvent(Signal::Timer);
    expect(sm.currentState() == TrafficLight::Yellow, "expected Yellow");

    sm.handleEvent(Signal::Timer);
    expect(sm.currentState() == TrafficLight::Red, "expected Red");
}

void test_transition_action() {
    state_machine::StateMachine<TrafficLight, Signal> sm;

    std::string log;
    sm.setState(TrafficLight::Red);
    sm.addTransition(TrafficLight::Red, Signal::Timer, TrafficLight::Green,
        [&log]() { log += "red->green;"; });
    sm.addTransition(TrafficLight::Green, Signal::Timer, TrafficLight::Yellow,
        [&log]() { log += "green->yellow;"; });

    sm.handleEvent(Signal::Timer);
    sm.handleEvent(Signal::Timer);

    expect(log == "red->green;green->yellow;", "expected actions to be called");
}

void test_can_handle() {
    state_machine::StateMachine<TrafficLight, Signal> sm;

    sm.setState(TrafficLight::Red);
    sm.addTransition(TrafficLight::Red, Signal::Timer, TrafficLight::Green);

    expect(sm.canHandle(Signal::Timer), "expected can handle Timer");
}

void test_available_events() {
    state_machine::StateMachine<TrafficLight, Signal> sm;

    sm.setState(TrafficLight::Red);
    sm.addTransition(TrafficLight::Red, Signal::Timer, TrafficLight::Green);

    auto events = sm.availableEvents();
    expect(events.size() == 1, "expected 1 available event");
    expect(events[0] == Signal::Timer, "expected Timer");
}

void test_set_state() {
    state_machine::StateMachine<TrafficLight, Signal> sm;

    sm.setState(TrafficLight::Green);
    expect(sm.currentState() == TrafficLight::Green, "expected Green");
}

void test_invalid_transition() {
    state_machine::StateMachine<TrafficLight, Signal> sm;

    sm.setState(TrafficLight::Red);
    // No transition defined

    bool threw = false;
    try {
        sm.handleEvent(Signal::Timer);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "expected exception for invalid transition");
}

void test_multiple_events() {
    enum class Event { Start, Stop, Reset };

    state_machine::StateMachine<std::string, Event> sm;
    sm.setState("idle");

    sm.addTransition("idle", Event::Start, "running");
    sm.addTransition("running", Event::Stop, "idle");
    sm.addTransition("running", Event::Reset, "idle");

    sm.handleEvent(Event::Start);
    expect(sm.currentState() == "running", "expected running");

    sm.handleEvent(Event::Stop);
    expect(sm.currentState() == "idle", "expected idle");
}

void test_complex_state_machine() {
    enum class OrderState { Created, Processing, Shipped, Delivered, Cancelled };
    enum class OrderEvent { Ship, Deliver, Cancel };

    state_machine::StateMachine<OrderState, OrderEvent> sm;
    sm.setState(OrderState::Created);

    sm.addTransition(OrderState::Created, OrderEvent::Ship, OrderState::Processing);
    sm.addTransition(OrderState::Processing, OrderEvent::Ship, OrderState::Shipped);
    sm.addTransition(OrderState::Processing, OrderEvent::Cancel, OrderState::Cancelled);
    sm.addTransition(OrderState::Shipped, OrderEvent::Deliver, OrderState::Delivered);

    sm.handleEvent(OrderEvent::Ship);
    expect(sm.currentState() == OrderState::Processing, "expected Processing");

    sm.handleEvent(OrderEvent::Ship);
    expect(sm.currentState() == OrderState::Shipped, "expected Shipped");

    sm.handleEvent(OrderEvent::Deliver);
    expect(sm.currentState() == OrderState::Delivered, "expected Delivered");
}

} // namespace

int main() {
    try {
        std::cout << "State Machine Tests:\n";
        test_basic_transition();
        std::cout << "  test_basic_transition: PASS\n";

        test_transition_action();
        std::cout << "  test_transition_action: PASS\n";

        test_can_handle();
        std::cout << "  test_can_handle: PASS\n";

        test_available_events();
        std::cout << "  test_available_events: PASS\n";

        test_set_state();
        std::cout << "  test_set_state: PASS\n";

        test_invalid_transition();
        std::cout << "  test_invalid_transition: PASS\n";

        test_multiple_events();
        std::cout << "  test_multiple_events: PASS\n";

        test_complex_state_machine();
        std::cout << "  test_complex_state_machine: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "state_machine_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nstate_machine_tests passed" << std::endl;
    return 0;
}
