# 备忘录模式

一个备忘录模式实现，用于状态快照。

## 学习目标

- 理解备忘录模式设计
- 练习状态保存
- 学习撤销/重做功能
- 实现状态快照

## 非目标

- 构建生产级备忘录框架
- 实现复杂的状态管理
- 支持分布式备忘录

## 功能

- 带 type/restore 的 Editor 发起者
- 用于状态快照的 EditorMemento
- 用于撤销的 EditorHistory 管理者
- 状态保存
- 撤销功能

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/memento_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "memento_pattern.hpp"

memento_pattern::Editor editor;
memento_pattern::EditorHistory history;

editor.type("Hello");
history.saveState(editor.save());

editor.type(" World");
auto memento = history.undo();
editor.restore(memento);
```

## 建议的下一步

- 添加更多备忘录类型
- 实现备忘录压缩
- 添加备忘录验证
- 实现分布式备忘录
