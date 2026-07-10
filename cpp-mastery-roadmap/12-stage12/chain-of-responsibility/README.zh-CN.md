# 责任链模式

一个责任链模式实现，用于请求处理。

## 学习目标

- 理解责任链模式设计
- 练习请求处理链
- 学习处理器链接
- 实现灵活的请求处理

## 非目标

- 构建生产级处理器框架
- 实现复杂的处理器层次结构
- 支持分布式处理器

## 功能

- 带 content 和 priority 的 Request 结构
- 带链接的 Handler 接口
- 具体处理器（Auth、Logging、Validation、Processing）
- 链组合
- 请求过滤

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/chain_of_responsibility_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "chain_of_responsibility.hpp"

auto auth = std::make_shared<chain_of_responsibility::AuthHandler>();
auto logging = std::make_shared<chain_of_responsibility::LoggingHandler>();
auto processing = std::make_shared<chain_of_responsibility::ProcessingHandler>();

auth->setNext(logging);
logging->setNext(processing);

chain_of_responsibility::Request req{"test", 1};
auth->handle(req);
```

## 建议的下一步

- 添加更多处理器类型
- 实现处理器优先级
- 添加处理器缓存
- 实现分布式处理器
