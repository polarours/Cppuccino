# 命令批处理

一个命令批处理实现，用于分组多个命令。

## 学习目标

- 理解命令批处理
- 练习分组操作
- 学习原子批处理执行
- 实现批处理撤销

## 非目标

- 构建生产级批处理系统
- 实现复杂的批处理操作
- 支持分布式批处理

## 功能

- 带 execute/undo 的命令接口
- Increment/Decrement 具体命令
- 用于分组的 BatchCommand
- 原子批处理执行
- 逆序撤销

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/command_batch_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

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
