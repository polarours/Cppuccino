# 单例模式 (Singleton Pattern)

## 什么是单例模式？

单例模式（Singleton Pattern）保证一个类**只有一个实例**，并提供全局访问点。

> **核心思想**：把构造函数私有化，用 `getInstance()` 返回同一个对象。

## 适用场景

- 日志器（Logger）——全进程一份
- 配置中心（Config）——全进程一致
- 线程池、连接池、调度器

## 核心实现（Meyers Singleton）

C++11 之后最推荐的方式是 **Meyers Singleton**：函数内 `static` 变量在首次调用时构造，线程安全由 C++ 运行时保证（"Magic Statics"）。

```cpp
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;   // C++11 起线程安全
        return instance;
    }
    void log(const std::string& msg) { /* ... */ }
private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
```

## 三种实现对比

| 实现 | 线程安全 | 延迟构造 | 推荐度 |
|---|---|---|---|
| 函数内 `static`（Meyers） | ✅ C++11 起 | ✅ | ★★★★★ |
| `std::call_once` + `std::once_flag` | ✅ | ✅ | ★★★ |
| 饿汉（指针 + 手动 init） | 取决于实现 | ❌ | ★ |

**不要**自己写双检查锁（DCLP）——C++11 之前需要正确使用 `volatile` + 内存屏障才能安全，复杂度极高；C++11 之后 Meyers 已经是零成本最优解。

## 代码实现

```cpp
#include <iostream>
#include <map>
#include <mutex>
#include <string>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[LOG] " << message << "\n";
        ++count_;
    }
    int getLogCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    int count_ = 0;
    mutable std::mutex mutex_;
};

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }
    void set(const std::string& k, const std::string& v) { config_[k] = v; }
    std::string get(const std::string& k, const std::string& def = "") const {
        auto it = config_.find(k);
        return it != config_.end() ? it->second : def;
    }
private:
    Config() = default;
    std::map<std::string, std::string> config_;
};
```

完整可运行示例见 [singleton-pattern 项目](../../cpp-mastery-roadmap/10-stage10/singleton-pattern)。

## 陷阱与最佳实践

1. **删除拷贝构造和赋值**：避免 `Logger a = Logger::getInstance();` 破坏唯一性
2. **不要在析构中访问其他单例**：单例析构顺序未定义（`std::exit` 调用 `atexit` 顺序），跨单例访问会悬空
3. **不要把单例当全局变量用**：单例是受控的全局，比 `int g_count = 0;` 略好；优先用显式依赖注入
4. **可测试性代价**：测试时需要"重置单例"或"注入替身"——Meyers 写法让 reset 比较麻烦，常见做法是 `clear()` 方法或在测试中换用接口
5. **进程内 vs 跨进程**：单例只保证**一个进程**内唯一；多进程场景需要额外机制（共享内存/文件锁）

## 何时不用单例

- 对象有多个合理实例（数据库连接池里的多个连接）
- 仅作为参数传递——直接放进构造参数
- 测试中需要替换——优先用接口 + 工厂

## 总结

Meyers Singleton 是 C++11 之后最简洁、最安全的单例实现；但单例本身是"受控的全局"，应只用于真正需要进程唯一且无合理替代的场景。
