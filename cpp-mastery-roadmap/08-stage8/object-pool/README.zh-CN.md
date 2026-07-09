# 对象池

一个通用的对象池，用于复用昂贵的对象。

## 学习目标

- 理解对象复用模式
- 练习带自定义删除器的 shared_ptr
- 学习池管理和生命周期
- 实现线程安全的对象获取

## 非目标

- 构建生产级对象池
- 实现连接特定功能
- 支持复杂的驱逐策略

## 功能

- 基于模板的通用池
- 带自定义删除器的 shared_ptr 自动归还
- 线程安全的获取/释放
- 可配置的初始池大小
- 清空操作

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/object_pool_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "object_pool.hpp"

object_pool::ObjectPool<Connection> pool(
    []() { return new Connection(); },
    [](Connection* p) { delete p; },
    5  // 初始大小
);

auto conn = pool.acquire();
conn->query("SELECT * FROM users");
// shared_ptr 销毁时自动归还到池中
```

## 建议的下一步

- 添加对象复用前的验证
- 实现对象健康检查
- 添加池统计（命中率、未命中率）
- 实现池预热策略
