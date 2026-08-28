# 推辞模式 (Balking Pattern)

## 什么是推辞模式？

推辞模式（Balking Pattern）让一个对象在"不满足前置条件"时**直接返回**，既不阻塞、也不抛错、也不重试——调用方视之为"什么都没发生"。

> **核心思想**：用一个守卫检查（guard check）决定是否继续；条件不满足则立刻退出。

## 与 Guarded Suspension 的关系

| | Balking | Guarded Suspension |
|---|---|---|
| 条件不满足时 | 立即返回 | 等待条件成立 |
| 调用方拿到 | "无操作" | 成功的执行结果 |
| 适用场景 | 一次性机会、可丢失的事件 | 必须完成的请求 |
| 复杂度 | 低（无需条件变量） | 中（条件变量 + 锁） |

## 核心概念

- **守卫（Guard）**：在方法入口检查状态字段
- **推辞（Balks）**：守卫失败时 `return`（不抛、不重试）
- **幂等性**：重复调用是安全的，因为重复推辞不会有副作用

## 适用场景

- 设备已经打开，再次 `open()` 是 no-op
- 数据未就绪时跳过处理（流式场景中允许丢帧）
- 用户已经登录，再次点登录按钮无反应
- 单次触发型事件（按钮按下后短时间内重复按下被忽略）

## 问题场景

```cpp
// 问题：用异常表达"重复打开"
class Document {
    void open() {
        if (opened_) throw AlreadyOpen{};
        opened_ = true;
    }
};
// 调用方必须 try-catch，可读性差且代价高
```

### 解决方案

```cpp
class Document {
public:
    void open() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (opened_) return;     // 静默推辞
        opened_ = true;
    }
    void save() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;    // 没打开就别保存
        saved_ = true;
    }
private:
    std::mutex mutex_;
    bool opened_ = false;
    bool saved_ = false;
};
```

## 代码实现

```cpp
#include <atomic>
#include <mutex>

class Document {
public:
    void open() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (opened_) return;
        opened_ = true;
    }
    void save() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;
        saved_ = true;
    }
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;
        opened_ = false;
        saved_ = false;
    }
    bool isOpen() const { return opened_; }
    bool isSaved() const { return saved_; }

private:
    std::mutex mutex_;
    bool opened_ = false;
    bool saved_ = false;
};

class Sensor {
public:
    explicit Sensor(int threshold = 100) : threshold_(threshold) {}

    // 无锁版本：单生产者单消费者
    bool read(int value) {
        if (value > threshold_) {
            lastValue_ = value;
            triggered_ = true;
            return true;
        }
        return false;             // 不触发即推辞
    }
    bool wasTriggered() const { return triggered_; }
    int lastValue() const { return lastValue_; }

private:
    int threshold_;
    std::atomic<bool> triggered_{false};
    std::atomic<int> lastValue_{0};
};
```

完整可运行示例见 [balking 项目](../../cpp-mastery-roadmap/31-stage31/balking)。

## 最佳实践

1. **文档化推辞语义**：调用方必须知道某次调用可能是"无操作"——方法名最好暗示（如 `tryOpen`）
2. **守卫检查与状态修改要在同一锁内**：避免 TOCTOU 竞争
3. **不要把推辞当静默失败**：在关键路径上，记录一次推辞便于排障
4. **幂等性是推辞的前提**：被推辞的方法在重复调用下不应留下脏状态

## 总结

Balking 以"无操作"代替"阻塞或异常"，是处理幂等操作和短窗口防抖的简洁工具；正确使用时能让 API 更宽容，但要确保调用方明确知道"这次什么都没发生"。
