# 命令历史

一个命令历史管理器，用于日志和重放。

## 学习目标

- 理解命令历史模式
- 练习命令日志
- 学习撤销功能
- 实现历史管理

## 非目标

- 构建生产级历史系统
- 实现复杂的重放逻辑
- 支持分布式历史

## 功能

- 带时间戳的 CommandEntry
- 带最大大小的 HistoryManager
- 撤销功能
- 清空操作
- 命令日志

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/command_history_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

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
