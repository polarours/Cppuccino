# Double Dispatch

A double dispatch pattern for multi-method polymorphism.

## Learning Goals

- Understand double dispatch pattern
- Practice multi-method polymorphism
- Learn visitor pattern extensions
- Implement game logic

## Non-Goals

- Building a production game engine
- Implementing complex game rules
- Supporting distributed games

## Features

- Element interface with accept
- Rock, Paper, Scissors elements
- Game visitor for results
- Double dispatch function
- Game logic

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/double_dispatch_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "double_dispatch.hpp"

double_dispatch::Rock rock;
double_dispatch::Paper paper;

std::string result = double_dispatch::playGame(rock, paper);
```

## 建议的下一步

- 添加更多游戏元素
- 实现得分系统
- 添加游戏历史
- 实现网络对战
