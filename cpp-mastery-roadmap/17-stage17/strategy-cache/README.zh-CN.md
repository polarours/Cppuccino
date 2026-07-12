# 策略缓存

一个带可配置策略（LRU、TTL）的缓存。

## 学习目标

- 理解缓存策略模式
- 练习缓存中的策略模式
- 学习 LRU 和 TTL 策略
- 实现策略切换

## 非目标

- 构建生产级缓存
- 实现复杂的驱逐策略
- 支持分布式缓存

## 功能

- CacheStrategy 接口
- 带容量的 LRU 策略
- 带过期的 TTL 策略
- 用于策略切换的 Cache 上下文
- 线程安全操作

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/strategy_cache_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "strategy_cache.hpp"

strategy_cache::Cache<std::string, int> cache;
cache.setStrategy(std::make_shared<strategy_cache::LRUStrategy<std::string, int>>(100));

cache.put("key", 42);
auto val = cache.get("key");
```

## 建议的下一步

- 添加更多缓存策略
- 实现策略组合
- 添加缓存统计
- 实现分布式缓存
