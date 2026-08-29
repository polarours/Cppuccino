# 观察者模式 (Observer Pattern)

## 什么是观察者模式？

观察者模式（Observer Pattern）定义对象间的一种**一对多依赖**：当"主题"（Subject）状态变化时，所有"观察者"（Observer）都会收到通知。

> **核心思想**：主题不直接调用观察者，而是通过接口广播；观察者自行订阅/退订。

## 适用场景

- 事件总线（UI 更新、消息分发）
- 实时数据推送（行情、心跳）
- 模型-视图同步（MVC 中的 View 订阅 Model）
- 状态变化触发多个独立处理（缓存失效 + 日志 + 告警）

## 核心概念

- **Subject（主题）**：维护观察者列表，提供 `attach / detach / notify`
- **Observer（观察者）**：实现 `update(value)` 接口
- **弱引用 vs 强引用**：观察者用 `weak_ptr` 存储避免循环引用，Subject 不延长观察者寿命
- **拉模型 vs 推模型**：推模型 `update(value)` 携带数据；拉模型只通知变化，Observer 主动 `subject.getValue()`

## 与发布-订阅的对比

| | Observer | Pub/Sub |
|---|---|---|
| 中介 | 无（直接调用） | 事件总线/消息中间件 |
| 耦合 | Subject 知道 Observer 类型 | 发布者和订阅者互不知道 |
| 同步 | 通常同步 | 经常异步 |
| 范围 | 进程内 | 可跨进程/跨网络 |

## 代码实现

```cpp
#include <algorithm>
#include <memory>
#include <vector>

template <typename T>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const T& value) = 0;
};

template <typename T>
class Subject {
public:
    void attach(std::shared_ptr<Observer<T>> observer) {
        observers_.push_back(std::move(observer));
    }

    void detach(const std::shared_ptr<Observer<T>>& observer) {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end());
    }

    void notify(const T& value) {
        // 拷贝列表：避免回调中修改原列表
        auto snapshot = observers_;
        for (auto& w : snapshot) {
            if (auto sp = w.lock()) sp->update(value);
        }
    }

    void setValue(T value) {
        value_ = value;
        notify(value_);
    }
    T getValue() const { return value_; }

private:
    T value_{};
    std::vector<std::weak_ptr<Observer<T>>> observers_;
};

template <typename T>
class Logger : public Observer<T> {
public:
    void update(const T& v) override { /* std::cout << v << '\n'; */ }
private:
    int updates_ = 0;
};
```

完整可运行示例见 [observer-pattern 项目](../../cpp-mastery-roadmap/08-stage8/observer-pattern)。

## 使用示例

```cpp
Subject<int> temperature;
auto display = std::make_shared<Logger<int>>();
auto alarm   = std::make_shared<Logger<int>>();

temperature.attach(display);
temperature.attach(alarm);

temperature.setValue(42);    // 触发 display.update + alarm.update
temperature.detach(alarm);
temperature.setValue(43);    // 只剩 display
```

## 关键陷阱

1. **观察者列表在通知中被修改**：迭代过程中 `attach/detach` 会使迭代器失效。务必先 `snapshot` 拷贝
2. **观察者持有 Subject 强引用** → 循环引用。用 `weak_ptr` 解决
3. **异常传播**：一个观察者 `throw`，后续观察者不会被通知。把每个 `update` 包在 `try/catch` 里
4. **生命周期错位**：Subject 析构后仍被观察者访问会悬空——`weak_ptr.lock()` 失败时跳过
5. **顺序依赖**：观察者之间的调用顺序应被视为未定义，逻辑上不能相互依赖

## 何时不用

- 状态变化只触发一个处理 → 直接调用
- 跨进程、跨语言 → 用真正的消息队列（ZeroMQ / Kafka）
- 一对一的通知 → 函数指针 / `std::function`

## 总结

观察者模式是事件驱动架构的最小单元；正确实现的核心是"列表拷贝 + 弱引用 + 异常隔离"，三者缺一就会在生产中暴露稳定性问题。
