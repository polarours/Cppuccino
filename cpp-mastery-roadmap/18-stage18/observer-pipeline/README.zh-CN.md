# 观察者管道

一个用于链式数据处理的管道。

## 学习目标

- 理解管道模式
- 练习数据转换链
- 学习阶段组合
- 实现顺序处理

## 非目标

- 构建生产级管道
- 实现复杂的转换
- 支持分布式管道

## 功能

- 带阶段的管道模板
- 阶段组合
- 字符串处理辅助函数
- 顺序处理
- 类型安全的管道

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/observer_pipeline_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "observer_pipeline.hpp"

observer_pipeline::Pipeline<std::string> pipeline;
pipeline.addStage(observer_pipeline::trim);
pipeline.addStage(observer_pipeline::toUpper);

std::string result = pipeline.process("  hello  ");
// result == "HELLO"
```

## 建议的下一步

- 添加并行处理阶段
- 实现错误处理
- 添加中间结果查看
- 实现管道缓存
