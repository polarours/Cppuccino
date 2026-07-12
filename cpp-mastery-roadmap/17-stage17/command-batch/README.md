# Command Batch

A command batch implementation for grouping multiple commands.

## Learning Goals

- Understand command batching
- Practice group operations
- Learn atomic batch execution
- Implement batch undo

## Non-Goals

- Building a production batch system
- Implementing complex batch operations
- Supporting distributed batches

## Features

- Command interface with execute/undo
- Increment/Decrement concrete commands
- BatchCommand for grouping
- Atomic batch execution
- Reverse order undo

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/command_batch_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "command_batch.hpp"

int counter = 0;
auto batch = std::make_shared<command_batch::BatchCommand>();
batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
batch->add(std::make_shared<command_batch::IncrementCommand>(counter));

batch->execute();  // counter = 2
batch->undo();     // counter = 0
```

## 建议的下一步

- 添加更多命令类型
- 实现条件批处理
- 添加批处理日志
- 实现分布式批处理
