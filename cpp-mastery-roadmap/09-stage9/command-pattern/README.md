# Command Pattern

A command pattern implementation with undo/redo support.

## Learning Goals

- Understand command pattern design
- Practice undo/redo functionality
- Learn object encapsulation of actions
- Implement command history management

## Non-Goals

- Building a production command system
- Implementing complex command composition
- Supporting distributed commands

## Features

- Command interface with execute/undo
- Concrete commands for text operations
- Command history with undo/redo stacks
- Thread-safe command execution

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/command_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "command_pattern.hpp"

command_pattern::TextEditor editor;
command_pattern::CommandHistory history;

history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "Hello"));
history.undo();  // Undo insert
history.redo();  // Redo insert
```

## Suggested Next Steps

- Add macro commands (group multiple commands)
- Implement command serialization
- Add command compression
- Implement distributed command execution
