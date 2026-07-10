# 中介者模式

一个中介者模式实现，用于对象通信。

## 学习目标

- 理解中介者模式设计
- 练习对象间的松耦合
- 学习集中式通信
- 实现基于事件的消息传递

## 非目标

- 构建生产级中介者框架
- 实现复杂的中介者层次结构
- 支持分布式中介者

## 功能

- 带 notify 的 Mediator 接口
- Colleague 基类
- ChatRoom 具体中介者
- User 具体同事类
- 集中式通信

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/mediator_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "mediator_pattern.hpp"

auto chatRoom = std::make_shared<mediator_pattern::ChatRoom>();
auto user1 = std::make_shared<mediator_pattern::User>(chatRoom, "Alice");
auto user2 = std::make_shared<mediator_pattern::User>(chatRoom, "Bob");

chatRoom->addColleague(user1);
chatRoom->addColleague(user2);

user1->send("Hello!");  // 通知所有其他同事
```

## 建议的下一步

- 添加更多中介者类型
- 实现消息过滤
- 添加消息历史
- 实现分布式中介者
