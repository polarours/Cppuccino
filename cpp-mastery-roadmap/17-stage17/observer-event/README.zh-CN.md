# 观察者事件系统

一个观察者事件系统，用于事件驱动架构。

## 学习目标

- 理解事件驱动架构
- 练习事件分发
- 学习处理器注册
- 实现事件发射

## 非目标

- 构建生产级事件系统
- 实现复杂的事件过滤
- 支持分布式事件

## 功能

- 带 type、source、data 的 Event 结构
- EventHandler 回调函数
- 用于注册和发射的 EventDispatcher
- 每个事件类型多个处理器
- 处理器计数跟踪

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/observer_event_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "observer_event.hpp"

observer_event::EventDispatcher dispatcher;
dispatcher.on("click", [](const observer_event::Event& e) {
    std::cout << "Clicked: " << e.data << "\n";
});

dispatcher.emit({"click", "button", "clicked"});
```

## 建议的下一步

- 添加更多事件类型
- 实现事件过滤
- 添加事件历史
- 实现分布式事件
