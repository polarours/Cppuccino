# 单例模式

一个单例模式实现，用于单实例管理。

## 学习目标

- 理解单例模式设计
- 练习线程安全初始化
- 学习延迟初始化
- 实现全局访问点

## 非目标

- 构建生产级单例框架
- 实现复杂的单例变体
- 支持分布式单例

## 功能

- 带互斥锁的线程安全单例
- 延迟初始化
- 带键值存储的 Config 单例
- 全局访问点
- 线程安全

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/singleton_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "singleton_pattern.hpp"

auto& logger = singleton_pattern::Logger::getInstance();
logger.log("Hello");

auto& config = singleton_pattern::Config::getInstance();
config.set("host", "localhost");
```

## 建议的下一步

- 添加单例生命周期管理
- 实现单例依赖注入
- 添加单例测试支持
- 实现单例序列化
