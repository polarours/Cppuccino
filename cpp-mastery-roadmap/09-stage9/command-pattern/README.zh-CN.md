# 命令模式

一个命令模式实现，支持撤销/重做功能。

## 学习目标

- 理解命令模式设计
- 练习撤销/重做功能
- 学习动作的对象封装
- 实现命令历史管理

## 非目标

- 构建生产级命令系统
- 实现复杂的命令组合
- 支持分布式命令

## 功能

- 带 execute/undo 的命令接口
- 文本操作的具体命令
- 带撤销/重做栈的命令历史
- 线程安全的命令执行

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/command_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "command_pattern.hpp"

command_pattern::TextEditor editor;
command_pattern::CommandHistory history;

history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "Hello"));
history.undo();  // 撤销插入
history.redo();  // 重做插入
```

## 建议的下一步

- 添加宏命令（组合多个命令）
- 实现命令序列化
- 添加命令压缩
- 实现分布式命令执行
