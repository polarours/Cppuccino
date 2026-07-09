# 观察者模式

一个观察者模式实现，用于事件通知。

## 学习目标

- 理解观察者模式设计
- 练习 weak_ptr 防止循环引用
- 学习事件通知系统
- 实现订阅者管理

## 非目标

- 构建生产级事件系统
- 实现分布式观察者
- 支持复杂的事件过滤

## 功能

- 基于模板的 subject 和 observer
- 每个 subject 多个 observer
- attach/detach 操作
- weak pointer 防止循环
- 自动清理过期的 observer

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/observer_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "observer_pattern.hpp"

observer_pattern::Subject<int> subject;

auto observer = std::make_shared<observer_pattern::Logger<int>>();
subject.attach(observer);

subject.setValue(42);  // Logger 接收通知
```

## 建议的下一步

- 添加事件过滤
- 实现基于优先级的通知
- 添加异步通知
- 实现分布式观察者
