# 连接池

一个通用的连接池实现，用于管理可复用资源。

## 学习目标

- 理解资源池化模式
- 练习并发访问的线程同步
- 学习连接生命周期管理
- 实现超时和错误处理

## 非目标

- 构建生产级连接池
- 实现连接健康检查
- 支持分布式连接池

## 功能

- 基于模板的通用池
- 可配置的最小/最大池大小
- 线程安全的获取/释放
- 连接复用
- 超时支持
- 优雅关闭

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/connection_pool_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "connection_pool.hpp"

connection_pool::ConnectionPool<Connection> pool(
    []() { return Connection(); },  // 创建器
    5, 10  // 最小/最大大小
);

auto conn = pool.acquire();
conn.query("SELECT * FROM users");
pool.release(conn);
```

## 建议的下一步

- 添加连接健康检查
- 实现连接验证
- 添加连接统计
- 实现连接重试逻辑
