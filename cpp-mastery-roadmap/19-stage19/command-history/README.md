# Command History

A command history manager for logging and replay.

## Learning Goals

- Understand command history patterns
- Practice command logging
- Learn undo functionality
- Implement history management

## Non-Goals

- Building a production history system
- Implementing complex replay logic
- Supporting distributed history

## Features

- CommandEntry with timestamp
- HistoryManager with max size
- Undo functionality
- Clear operation
- Command logging

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/command_history_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "command_history.hpp"

command_history::HistoryManager history;
history.add("ls -la", "file1.txt");
history.undo();
```

## 建议的下一步

- 添加命令搜索
- 实现命令过滤
- 添加命令分类
- 实现分布式历史
