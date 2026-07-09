# 线程安全缓存

一个通用的线程安全缓存，支持 TTL 和驱逐策略。

## 学习目标

- 理解缓存策略和数据结构
- 练习读写锁实现并发访问
- 学习基于 TTL 的过期机制
- 实现 LRU 驱逐

## 非目标

- 构建生产级缓存（如 Redis）
- 实现分布式缓存
- 支持复杂的驱逐策略

## 功能

- 基于模板的通用缓存
- 使用读写锁保证线程安全
- 基于 TTL 的过期
- 容量满时 LRU 驱逐
- getOrElse 和 getOrCreate 辅助函数

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/thread_safe_cache_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "thread_safe_cache.hpp"

thread_safe_cache::Cache<std::string, int> cache(1000);

// 带 TTL 设置
cache.set("session", 12345, std::chrono::minutes(30));

// 带默认值获取
int value = cache.getOrElse("missing", 0);

// 获取或创建
int count = cache.getOrCreate("counter", []() { return 0; });
```

## 建议的下一步

- 添加命中/未命中统计
- 实现不同的驱逐策略（LFU、FIFO）
- 添加缓存预热策略
- 实现分布式缓存协议
