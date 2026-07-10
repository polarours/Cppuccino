# 建造者模式

一个建造者模式实现，用于复杂对象构建。

## 学习目标

- 理解建造者模式设计
- 练习逐步构建
- 学习 director 和 builder 分离
- 实现流式接口

## 非目标

- 构建生产级建造者框架
- 实现复杂的建造者层次结构
- 支持分布式建造者

## 功能

- 带组件的 Computer 产品
- 带构建步骤的 Builder 接口
- 具体建造者（Gaming、Office、Workstation）
- 用于构建序列的 Director
- 共享指针结果管理

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/builder_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "builder_pattern.hpp"

builder_pattern::ComputerDirector director;
director.setBuilder(std::make_shared<builder_pattern::GamingComputerBuilder>());
auto computer = director.construct();
```

## 建议的下一步

- 添加更多构建步骤
- 实现构建器验证
- 添加构建器缓存
- 实现构建器序列化
