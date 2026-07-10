# Mediator Pattern

A mediator pattern implementation for object communication.

## Learning Goals

- Understand mediator pattern design
- Practice loose coupling between objects
- Learn centralized communication
- Implement event-based messaging

## Non-Goals

- Building a production mediator framework
- Implementing complex mediator hierarchies
- Supporting distributed mediators

## Features

- Mediator interface with notify
- Colleague base class
- ChatRoom concrete mediator
- User concrete colleague
- Centralized communication

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/mediator_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "mediator_pattern.hpp"

auto chatRoom = std::make_shared<mediator_pattern::ChatRoom>();
auto user1 = std::make_shared<mediator_pattern::User>(chatRoom, "Alice");
auto user2 = std::make_shared<mediator_pattern::User>(chatRoom, "Bob");

chatRoom->addColleague(user1);
chatRoom->addColleague(user2);

user1->send("Hello!");  // Notifies all other colleagues
```

## 建议的下一步

- 添加更多中介者类型
- 实现消息过滤
- 添加消息历史
- 实现分布式中介者
