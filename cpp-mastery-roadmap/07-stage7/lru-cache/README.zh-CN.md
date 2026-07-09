# LRU 缓存

一个线程安全的最近最少使用（LRU）缓存实现。

## 学习目标

- 理解 LRU 驱逐策略
- 练习缓存操作的线程同步
- 学习缓存数据结构（哈希表 + 双向链表）
- 实现缓存命中/未命中模式

## 非目标

- 构建生产级缓存
- 实现分布式缓存
- 支持复杂的驱逐策略

## 功能

- O(1) 的 get 和 put 操作
- 容量满时 LRU 驱逐
- 使用互斥锁保证线程安全
- 包含和大小查询
- 清空操作

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/lru_cache_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "lru_cache.hpp"

lru_cache::LRUCache<std::string, int> cache(100);

cache.put("key1", 42);
auto value = cache.get("key1");  // 42

cache.put("key2", 100);
cache.get("key1");  // key1 现在是最近使用的

// 添加新条目会驱逐 key2（最近最少使用）
cache.put("key3", 200);
```

## 建议的下一步

- 添加命中/未命中统计
- 实现 TTL 支持
- 添加缓存预热策略
- 实现分布式 LRU 缓存
