# Interpreter Pattern

A simple arithmetic expression interpreter.

## Learning Goals

- Understand interpreter pattern design
- Practice AST construction
- Learn expression evaluation
- Implement recursive interpretation

## Non-Goals

- Building a production interpreter
- Implementing complex grammar parsing
- Supporting full language features

## Features

- Expression interface with interpret
- Number, Variable, Add, Subtract, Multiply expressions
- Nested expression support
- Variable context
- toString for debugging

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/interpreter_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "interpreter_pattern.hpp"

auto expr = std::make_shared<interpreter_pattern::AddExpression>(
    std::make_shared<interpreter_pattern::NumberExpression>(3),
    std::make_shared<interpreter_pattern::NumberExpression>(4)
);

int result = expr->interpret();  // 7
```

## 建议的下一步

- 添加更多操作符（除法、取模）
- 实现语法解析器
- 添加表达式简化
- 实现表达式缓存
