# 状态模式 (State Pattern)

## 什么是状态模式？

状态模式（State Pattern）是一种行为型设计模式，它允许一个对象在其内部状态改变时改变它的行为。对象看起来似乎修改了它的类。

> **核心思想**：将状态封装到独立的类中，让对象在运行时根据状态改变行为。

## 为什么需要状态模式？

### 问题场景

假设我们有一个订单处理系统：

```cpp
// 问题：大量的条件判断
class Order {
public:
    void process() {
        if (status_ == "PENDING") {
            // 处理待处理订单
            pay();
            status_ = "PAID";
        } else if (status_ == "PAID") {
            // 处理已支付订单
            ship();
            status_ = "SHIPPED";
        } else if (status_ == "SHIPPED") {
            // 处理已发货订单
            deliver();
            status_ = "DELIVERED";
        }
        // 更多状态...
    }

private:
    std::string status_;
    void pay() { /* ... */ }
    void ship() { /* ... */ }
    void deliver() { /* ... */ }
};
```

**问题**：
1. 条件分支过多
2. 添加新状态需要修改现有代码
3. 状态逻辑分散，难以维护

### 解决方案

使用状态模式：

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
    void pay() override { /* 处理支付 */ }
    void ship() override { /* 不允许发货 */ }
    void deliver() override { /* 不允许配送 */ }
};

class PaidState : public OrderState {
public:
    void pay() override { /* 已支付 */ }
    void ship() override { /* 处理发货 */ }
    void deliver() override { /* 不允许配送 */ }
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

**优点**：
1. 消除条件分支
2. 易于添加新状态
3. 状态逻辑清晰

## 模式结构

```
                    ┌─────────────┐
                    │   Context   │
                    │  (订单)      │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  State      │ │  State     │ │   State    │ ← 抽象状态
    │  (接口)     │ │  (接口)    │ │  (接口)    │
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Pending    │ │   Paid      │ │  Shipped   │ ← 具体状态
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义状态接口

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

### 2. 实现具体状态

```cpp
class RedState : public TrafficLightState {
public:
    void handle() override {
        std::cout << "红灯停止！等待绿 lights...\n";
    }
    
    std::string getStateName() const override {
        return "Red";
    }
};

class YellowState : public TrafficLightState {
public:
    void handle() override {
        std::cout << "黄灯准备！即将变绿...\n";
    }
    
    std::string getStateName() const override {
        return "Yellow";
    }
};

class GreenState : public TrafficLightState {
public:
    void handle() override {
        std::cout << "绿灯通行！\n";
    }
    
    std::string getStateName() const override {
        return "Green";
    }
};
```

### 3. 实现上下文

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

### 4. 状态转换

```cpp
class Order {
public:
    explicit Order() {
        setState(std::make_unique<PendingState>());
    }
    
    void pay() {
        if (auto* pending = dynamic_cast<PendingState*>(state_.get())) {
            std::cout << "处理支付...\n";
            setState(std::make_unique<PaidState>());
        }
    }
    
    void ship() {
        if (auto* paid = dynamic_cast<PaidState*>(state_.get())) {
            std::cout << "处理发货...\n";
            setState(std::make_unique<ShippedState>());
        }
    }
    
    void deliver() {
        if (auto* shipped = dynamic_cast<ShippedState*>(state_.get())) {
            std::cout << "处理配送...\n";
            setState(std::make_unique<DeliveredState>());
        }
    }
    
    void showStatus() const {
        std::cout << "订单状态: " << state_->getStateName() << "\n";
    }

private:
    void setState(std::unique_ptr<OrderState> state) {
        state_ = std::move(state);
    }
    
    std::unique_ptr<OrderState> state_;
};
```

## 现代 C++ 实现

### 使用 lambda 简化

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

### 使用状态机

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

### 使用 CRTP

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
    void doEnter() { std::cout << "进入红灯状态\n"; }
    void doExit() { std::cout << "退出红灯状态\n"; }
    void doHandle() { std::cout << "红灯停止\n"; }
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **策略模式** | 策略模式由客户端选择策略；状态模式由对象内部管理状态 |
| **状态 vs 状态机** | 状态机明确定义状态转换；状态模式更灵活 |
| **状态 vs 命令** | 命令封装请求；状态封装当前状态 |

## 最佳实践

### 1. 使用枚举表示状态

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

### 2. 使用状态转换表

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

### 3. 支持复合状态

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

## 常见陷阱

### 陷阱 1：状态转换错误

```cpp
// 不推荐：允许非法转换
void pay() {
    state_->pay();  // 任何状态都可以支付
}

// 推荐：只允许合法转换
void pay() {
    if (auto* pending = dynamic_cast<PendingState*>(state_.get())) {
        pending->pay();
        setState(std::make_unique<PaidState>());
    }
}
```

### 陷阱 2：状态对象过多

```cpp
// 不推荐：每个状态都创建一个类
class State1 { ... };
class State2 { ... };
class State3 { ... };
// 可能有几十个状态类

// 推荐：使用 lambda 或策略组合
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

### 陷阱 3：忘记清理状态

```cpp
// 不推荐：状态对象可能泄漏
class BadContext {
public:
    void setState(State* state) {
        delete state_;  // 忘记清理旧状态
        state_ = state;
    }
    
private:
    State* state_ = nullptr;
};

// 推荐：使用智能指针
class GoodContext {
public:
    void setState(std::unique_ptr<State> state) {
        state_ = std::move(state);
    }
    
private:
    std::unique_ptr<State> state_;
};
```

## 总结

状态模式的核心要点：

- **适用场景**：对象行为依赖状态、状态转换复杂、需要消除条件分支
- **核心优点**：消除条件判断、易于扩展、状态清晰
- **实现要点**：使用智能指针、支持状态转换、避免非法转换
- **常见陷阱**：状态转换错误、状态对象过多、内存泄漏

> **记住**：状态模式的关键是"状态封装"——将每个状态封装到独立的类中，让对象在状态改变时自动改变行为。
