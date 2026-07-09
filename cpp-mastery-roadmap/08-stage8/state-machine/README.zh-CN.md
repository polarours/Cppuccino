# 状态机

一个通用的状态机实现，用于建模状态转换。

## 学习目标

- 理解状态机设计模式
- 练习基于模板的泛型编程
- 学习状态转换管理
- 实现基于事件的状态变更

## 非目标

- 构建生产级状态机框架
- 实现层次状态机
- 支持复杂的状态机图

## 功能

- 基于模板的通用状态机
- 带动作的状态转换
- 查询可用事件
- 直接设置状态
- 线程安全设计

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/state_machine_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "state_machine.hpp"

enum class State { Idle, Running, Stopped };
enum class Event { Start, Stop };

state_machine::StateMachine<State, Event> sm;
sm.setState(State::Idle);
sm.addTransition(State::Idle, Event::Start, State::Running);
sm.addTransition(State::Running, Event::Stop, State::Stopped);

sm.handleEvent(Event::Start);  // 状态：Running
sm.handleEvent(Event::Stop);   // 状态：Stopped
```

## 建议的下一步

- 添加层次状态机
- 实现状态进入/退出动作
- 添加转换的守卫条件
- 实现状态机可视化
