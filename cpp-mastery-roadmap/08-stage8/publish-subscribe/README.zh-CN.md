# 发布-订阅

一个简单的发布-订阅消息系统。

## 学习目标

- 理解发布-订阅模式
- 练习基于回调的事件处理
- 学习基于主题的消息路由
- 实现订阅者管理

## 非目标

- 构建生产级消息代理
- 实现消息持久化
- 支持分布式消息

## 功能

- 基于主题的发布/订阅
- 每个主题多个订阅者
- 订阅/取消订阅操作
- 线程安全操作
- 消息顺序保证

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/pubsub_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "pubsub.hpp"

pubsub::Publisher<std::string> pub;

auto id = pub.subscribe("news", [](const std::string& msg) {
    std::cout << "News: " << msg << "\n";
});

pub.publish("news", "Breaking news!");

pub.unsubscribe(id);
```

## 建议的下一步

- 添加消息过滤
- 实现消息队列
- 添加订阅者优先级
- 实现分布式发布-订阅
