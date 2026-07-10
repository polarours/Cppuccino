# Memento Pattern

A memento pattern implementation for state snapshots.

## Learning Goals

- Understand memento pattern design
- Practice state preservation
- Learn undo/redo functionality
- Implement state snapshots

## Non-Goals

- Building a production memento framework
- Implementing complex state management
- Supporting distributed mementos

## Features

- Editor originator with type/restore
- EditorMemento for state snapshots
- EditorHistory caretaker for undo
- State preservation
- Undo functionality

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/memento_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

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
