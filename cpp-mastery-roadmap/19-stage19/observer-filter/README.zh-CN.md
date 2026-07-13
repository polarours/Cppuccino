# 观察者过滤器

一个带事件过滤的观察者模式。

## 学习目标

- 理解事件过滤
- 练习条件事件处理
- 学习过滤器组合
- 实现选择性通知

## 非目标

- 构建生产级过滤系统
- 实现复杂的过滤逻辑
- 支持分布式过滤

## 功能

- 带 type、data、priority 的 Event
- EventFilter 谓词
- 用于过滤的 FilteredDispatcher
- 多个过滤器支持
- 选择性通知

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/observer_filter_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "observer_filter.hpp"

observer_filter::FilteredDispatcher dispatcher;
dispatcher.addFilter([](const observer_filter::Event& e) {
    return e.priority > 5;
});

dispatcher.onEvent([](const observer_filter::Event& e) {
    std::cout << e.data << "\n";
});

dispatcher.emit({"click", "low", 3});   // 被过滤
dispatcher.emit({"click", "high", 10}); // 通过
```

## 建议的下一步

- 添加更多过滤器类型
- 实现过滤器链
- 添加过滤器统计
- 实现分布式过滤
