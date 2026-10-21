# State Pattern

## What Is the State Pattern?

The State Pattern is a behavioral design pattern that lets an object change its
behavior when its internal state changes — the object appears to change its
class.

> **Core idea**: encapsulate each state in its own class, so the object can
> change its behavior at runtime, based on the state it is in.

## Why Do We Need It?

### Problem Scenario

Suppose we have an order-processing system:

```cpp
// Problem: a pile of conditional branches
class Order {
public:
    void process() {
        if (status_ == "PENDING") {
            // handle a pending order
            pay();
            status_ = "PAID";
        } else if (status_ == "PAID") {
            // handle a paid order
            ship();
            status_ = "SHIPPED";
        } else if (status_ == "SHIPPED") {
            // handle a shipped order
            deliver();
            status_ = "DELIVERED";
        }
        // more states...
    }

private:
    std::string status_;
    void pay() { /* ... */ }
    void ship() { /* ... */ }
    void deliver() { /* ... */ }
};
```

**Problems**:

1. Too many conditional branches
2. Adding a new state means changing existing code
3. The state logic is scattered everywhere and hard to maintain

### Solution

Use the state pattern:

```cpp
class OrderState {
public:
    virtual ~OrderState() = default;
    virtual void pay() = 0;
    virtual void ship() = 0;
    virtual void deliver() = 0;
};

class PendingState : public OrderState {
public:
    void pay() override { /* handle the payment */ }
    void ship() override { /* shipping is not allowed */ }
    void deliver() override { /* delivery is not allowed */ }
};

class PaidState : public OrderState {
public:
    void pay() override { /* already paid */ }
    void ship() override { /* handle the shipment */ }
    void deliver() override { /* delivery is not allowed */ }
};

class Order {
public:
    void setState(std::unique_ptr<OrderState> state) {
        state_ = std::move(state);
    }
    
    void pay() { state_->pay(); }
    void ship() { state_->ship(); }
    void deliver() { state_->deliver(); }

private:
    std::unique_ptr<OrderState> state_;
};
```

**Benefits**:

1. Eliminates the conditional branches
2. Makes new states easy to add
3. Keeps the state logic clear

## Pattern Structure

```
                    ┌─────────────┐
                    │   Context   │
                    │   (order)   │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  State      │ │  State     │ │   State    │ ← abstract state
    │ (interface) │ │ (interface)│ │ (interface)│
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Pending    │ │   Paid      │ │  Shipped   │ ← concrete states
    └─────────────┘ └────────────┘ └────────────┘
```

## Implementation

### 1. Define the State Interface

```cpp
#include <memory>
#include <string>
#include <iostream>

class TrafficLightState {
public:
    virtual ~TrafficLightState() = default;
    virtual void handle() = 0;
    virtual std::string getStateName() const = 0;
};
```

### 2. Implement the Concrete States

```cpp
class RedState : public TrafficLightState {
public:
    void handle() override {
        std::cout << "Red light! Stop and wait for the green light...\n";
    }
    
    std::string getStateName() const override {
        return "Red";
    }
};

class YellowState : public TrafficLightState {
public:
    void handle() override {
        std::cout << "Yellow light! Get ready, turning green soon...\n";
    }
    
    std::string getStateName() const override {
        return "Yellow";
    }
};

class GreenState : public TrafficLightState {
public:
    void handle() override {
        std::cout << "Green light! Go!\n";
    }
    
    std::string getStateName() const override {
        return "Green";
    }
};
```

### 3. Implement the Context

```cpp
class TrafficLight {
public:
    explicit TrafficLight(std::unique_ptr<TrafficLightState> initialState)
        : state_(std::move(initialState)) {}
    
    void setState(std::unique_ptr<TrafficLightState> state) {
        state_ = std::move(state);
    }
    
    void handle() {
        state_->handle();
    }
    
    std::string getCurrentState() const {
        return state_->getStateName();
    }

private:
    std::unique_ptr<TrafficLightState> state_;
};
```

### 4. State Transitions

```cpp
class Order {
public:
    explicit Order() {
        setState(std::make_unique<PendingState>());
    }
    
    void pay() {
        if (auto* pending = dynamic_cast<PendingState*>(state_.get())) {
            std::cout << "Processing payment...\n";
            setState(std::make_unique<PaidState>());
        }
    }
    
    void ship() {
        if (auto* paid = dynamic_cast<PaidState*>(state_.get())) {
            std::cout << "Processing shipment...\n";
            setState(std::make_unique<ShippedState>());
        }
    }
    
    void deliver() {
        if (auto* shipped = dynamic_cast<ShippedState*>(state_.get())) {
            std::cout << "Processing delivery...\n";
            setState(std::make_unique<DeliveredState>());
        }
    }
    
    void showStatus() const {
        std::cout << "Order status: " << state_->getStateName() << "\n";
    }

private:
    void setState(std::unique_ptr<OrderState> state) {
        state_ = std::move(state);
    }
    
    std::unique_ptr<OrderState> state_;
};
```

## Modern C++ Implementations

### Simplifying with Lambdas

```cpp
class LambdaState {
public:
    using StateAction = std::function<void()>;
    
    LambdaState(StateAction action) : action_(std::move(action)) {}
    
    void execute() { action_(); }

private:
    StateAction action_;
};
```

### Using a State Machine

```cpp
template<typename StateType>
class StateMachine {
public:
    template<typename Transition>
    void addTransition(StateType from, StateType to, Transition transition) {
        transitions_[{from, to}] = std::move(transition);
    }
    
    bool transition(StateType from, StateType to) {
        auto it = transitions_.find({from, to});
        if (it != transitions_.end()) {
            it->second();
            return true;
        }
        return false;
    }

private:
    std::map<std::pair<StateType, StateType>, std::function<void()>> transitions_;
};
```

### Using CRTP

```cpp
template<typename Derived>
class StateBase {
public:
    void enter() {
        static_cast<Derived*>(this)->doEnter();
    }
    
    void exit() {
        static_cast<Derived*>(this)->doExit();
    }
    
    void handle() {
        static_cast<Derived*>(this)->doHandle();
    }

protected:
    ~StateBase() = default;
};

class RedLightState : public StateBase<RedLightState> {
public:
    void doEnter() { std::cout << "Entering the red light state\n"; }
    void doExit() { std::cout << "Leaving the red light state\n"; }
    void doHandle() { std::cout << "Red light: stop\n"; }
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Strategy** | The client picks the strategy; the state pattern lets the object manage its state internally |
| **State vs state machine** | A state machine defines the transitions explicitly; the state pattern is more flexible |
| **State vs command** | Command encapsulates a request; state encapsulates the current state |

## Best Practices

### 1. Represent States with an Enum

```cpp
enum class OrderStatus {
    PENDING,
    PAID,
    SHIPPED,
    DELIVERED
};

class Order {
public:
    void setState(OrderStatus status) {
        status_ = status;
    }
    
    OrderStatus getStatus() const { return status_; }

private:
    OrderStatus status_ = OrderStatus::PENDING;
};
```

### 2. Use a Transition Table

```cpp
class StateTransitionTable {
public:
    void addTransition(OrderStatus from, OrderStatus to, Action action) {
        transitions_[{from, to}] = action;
    }
    
    bool canTransition(OrderStatus from, OrderStatus to) const {
        return transitions_.find({from, to}) != transitions_.end();
    }
    
    void executeTransition(OrderStatus from, OrderStatus to) {
        transitions_[{from, to}]();
    }

private:
    std::map<std::pair<OrderStatus, OrderStatus>, std::function<void()>> transitions_;
};
```

### 3. Support Composite States

```cpp
class CompositeState : public State {
public:
    void addSubState(std::unique_ptr<State> state) {
        subStates_.push_back(std::move(state));
    }
    
    void enter() override {
        for (auto& sub : subStates_) {
            sub->enter();
        }
    }
    
    void exit() override {
        for (auto it = subStates_.rbegin(); it != subStates_.rend(); ++it) {
            it->exit();
        }
    }

private:
    std::vector<std::unique_ptr<State>> subStates_;
};
```

## Common Traps

### Trap 1: Wrong State Transitions

```cpp
// Not recommended: illegal transitions are allowed
void pay() {
    state_->pay();  // any state can pay
}

// Recommended: only allow legal transitions
void pay() {
    if (auto* pending = dynamic_cast<PendingState*>(state_.get())) {
        pending->pay();
        setState(std::make_unique<PaidState>());
    }
}
```

### Trap 2: Too Many State Objects

```cpp
// Not recommended: one class per state
class State1 { ... };
class State2 { ... };
class State3 { ... };
// there may be dozens of state classes

// Recommended: use lambdas or combine strategies
class SimpleState {
public:
    explicit SimpleState(std::function<void()> enter,
                        std::function<void()> exit,
                        std::function<void()> handle)
        : enter_(std::move(enter)), exit_(std::move(exit)), handle_(std::move(handle)) {}
    
    void enter() { enter_(); }
    void exit() { exit_(); }
    void handle() { handle_(); }

private:
    std::function<void()> enter_;
    std::function<void()> exit_;
    std::function<void()> handle_;
};
```

### Trap 3: Forgetting to Clean Up the State

```cpp
// Not recommended: the state object may leak
class BadContext {
public:
    void setState(State* state) {
        delete state_;  // delete the old state before replacing it
        state_ = state;
    }
    
private:
    State* state_ = nullptr;
};

// Recommended: use smart pointers
class GoodContext {
public:
    void setState(std::unique_ptr<State> state) {
        state_ = std::move(state);
    }
    
private:
    std::unique_ptr<State> state_;
};
```

## Summary

The key points of the state pattern:

- **When to use it**: object behavior depends on the state, state transitions
  are complex, conditional branches have to go
- **Main benefits**: eliminates conditional branches, easy to extend, the state
  logic stays clear
- **Implementation advice**: use smart pointers, support state transitions,
  reject illegal transitions
- **Common traps**: wrong transitions, too many state objects, memory leaks

> **Remember**: the key of the state pattern is *encapsulating state* — give
> every state its own class, so the object changes its behavior automatically
> when its state changes.
