# 解释器模式

一个简单的算术表达式解释器。

## 学习目标

- 理解解释器模式设计
- 练习 AST 构建
- 学习表达式求值
- 实现递归解释

## 非目标

- 构建生产级解释器
- 实现复杂的语法解析
- 支持完整的语言特性

## 功能

- 带 interpret 的 Expression 接口
- Number、Variable、Add、Subtract、Multiply 表达式
- 嵌套表达式支持
- 变量上下文
- 用于调试的 toString

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/interpreter_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

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
