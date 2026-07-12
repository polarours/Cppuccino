# 高级对象池

一个高级对象池，支持验证和健康检查。

## 学习目标

- 理解高级对象池化
- 练习对象验证
- 学习健康检查模式
- 实现健壮的池化

## 非目标

- 构建生产级池框架
- 实现复杂的验证规则
- 支持分布式池化

## 功能

- 基于模板的通用池
- 复用前的对象验证
- 健康检查支持
- 线程安全操作
- 可配置的池大小

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/object_pool_advanced_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "object_pool_advanced.hpp"

object_pool_advanced::ObjectPool<Connection> pool(
    []() { return new Connection(); },
    [](Connection* p) { delete p; },
    [](const Connection* p) { return p->valid; },
    2, 10
);

auto conn = pool.acquire();
// 使用连接
conn.reset();  // 归还到池
```

## 建议的下一步

- 添加更多验证规则
- 实现连接健康检查
- 添加池统计
- 实现池监控
