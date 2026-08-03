# 单例模式 (Singleton Pattern)

## 什么是单例模式？

单例模式（Singleton Pattern）是一种创建型设计模式，它确保一个类只有一个实例，并提供一个全局访问点。

> **核心思想**：控制实例数量，确保全局唯一性，提供统一的访问接口。

## 为什么需要单例？

### 问题场景

某些资源在全局范围内只需要一个实例：
- 数据库连接池
- 配置管理器
- 日志记录器
- 缓存系统

```cpp
// 问题：多个对象可能创建多个连接
Database db1 = createDatabase();  // 连接1
Database db2 = createDatabase();  // 连接2 - 重复！
```

### 解决方案

使用单例模式：

```cpp
// 正确：只创建一个实例
Database& db = Database::getInstance();
```

## 模式结构

```
                    ┌─────────────┐
                    │  Singleton  │
                    │  (Class)    │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ getInstance()│ │ instance_  │ │ 私有构造函数│
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 基本实现

### 经典实现（线程不安全）

```cpp
class Singleton {
public:
    // 删除拷贝和赋值
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    // 获取唯一实例
    static Singleton* getInstance() {
        if (instance_ == nullptr) {
            instance_ = new Singleton();
        }
        return instance_;
    }
    
    // 释放实例
    static void destroy() {
        delete instance_;
        instance_ = nullptr;
    }

private:
    // 私有构造函数
    Singleton() { /* 初始化 */ }
    
    // 唯一实例指针
    static Singleton* instance_;
};

// 静态成员初始化
Singleton* Singleton::instance_ = nullptr;
```

### 梅耶斯单例（推荐）

```cpp
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    //  Meyers' Singleton - C++11 起线程安全
    static Singleton& getInstance() {
        static Singleton instance;  // 首次调用时创建
        return instance;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
};

// 使用
Singleton::getInstance().doSomething();
```

**优点**：
- 线程安全（C++11 起）
- 自动内存管理
- 简洁易读

## 变体与扩展

### 变体 1：延迟初始化

```cpp
class ConfigManager {
public:
    static ConfigManager& getInstance() {
        if (!instance_) {
            instance_ = new ConfigManager();
            loadConfig();  // 延迟加载配置
        }
        return *instance_;
    }

private:
    static ConfigManager* instance_;
    void loadConfig() { /* 从文件加载 */ }
};
```

### 变体 2：注册表模式

```cpp
class PluginManager {
public:
    static PluginManager& getInstance() {
        static PluginManager instance;
        return instance;
    }
    
    void registerPlugin(const std::string& name, std::unique_ptr<Plugin> plugin) {
        plugins_[name] = std::move(plugin);
    }
    
    Plugin* getPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        return it != plugins_.end() ? it->second.get() : nullptr;
    }

private:
    PluginManager() = default;
    std::map<std::string, std::unique_ptr<Plugin>> plugins_;
};
```

### 变体 3：线程局部单例

```cpp
class ThreadLocalConfig {
public:
    // 每个线程一个实例
    static ThreadLocalConfig& get() {
        thread_local ThreadLocalConfig instance;
        return instance;
    }

private:
    ThreadLocalConfig() = default;
};
```

## 最佳实践

### 1. 使用 Meyers' Singleton

```cpp
// 推荐：梅耶斯单例
static Singleton& getInstance() {
    static Singleton instance;
    return instance;
}

// 不推荐：手动管理内存
static Singleton* getInstance() {
    if (instance_ == nullptr) {
        instance_ = new Singleton();
    }
    return instance_;
}
```

### 2. 考虑使用 std::once_flag

```cpp
#include <mutex>

class Singleton {
public:
    static Singleton& getInstance() {
        std::call_once(flag_, [] {
            instance_ = new Singleton();
        });
        return *instance_;
    }
    
    static void destroy() {
        delete instance_;
        instance_ = nullptr;
    }

private:
    Singleton() = default;
    static Singleton* instance_;
    static std::once_flag flag_;
};
```

### 3. 使用智能指针

```cpp
#include <memory>

class Singleton {
public:
    static Singleton& getInstance() {
        static std::unique_ptr<Singleton> instance{new Singleton()};
        return *instance;
    }

private:
    Singleton() = default;
};
```

### 4. 考虑是否真的需要单例

```cpp
// 问题：单例可能成为全局状态
class Logger {
public:
    static Logger& getInstance() { ... }
};

// 替代：依赖注入
class Application {
public:
    explicit Application(std::unique_ptr<Logger> logger)
        : logger_(std::move(logger)) {}

private:
    std::unique_ptr<Logger> logger_;
};
```

## 常见陷阱

### 陷阱 1：线程安全问题

```cpp
// 不推荐：多线程下可能创建多个实例
static Singleton* getInstance() {
    if (instance_ == nullptr) {  // 检查点 A
        instance_ = new Singleton();  // 创建点 B
    }
    return instance_;
}

// 问题：线程 1 在 A 处检查后，线程 2 也可能进入
```

**解决方案**：使用 Meyers' Singleton 或双重检查锁定

### 陷阱 2：静态初始化顺序问题

```cpp
// 危险：不同 TUs 中的静态对象初始化顺序不确定
class A {
public:
    static A& getInstance() {
        static A instance;
        return instance;
    }
};

class B {
public:
    B() {
        A::getInstance().doSomething();  // 可能 A 还未初始化！
    }
    static B instance;
};

B B::instance;  // 初始化顺序不确定
```

**解决方案**：使用函数局部静态变量

### 陷阱 3：滥用单例

```cpp
// 不推荐：把所有东西都做成单例
class Database {
public:
    static Database& getInstance() { ... }
};

class UserService {
public:
    static UserService& getInstance() { ... }
};

// 应该：只在真正需要时使用时例
```

### 陷阱 4：单例与继承冲突

```cpp
// 问题：派生类也会成为单例
class Base {
public:
    static Base& getInstance() {
        static Base instance;
        return instance;
    }
};

class Derived : public Base {
public:
    static Derived& getInstance() {
        static Derived instance;  // 这是不同的单例！
        return instance;
    }
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **静态类** | 静态类无法继承，单例可以 |
| **注册表** | 注册表管理多个实例，单例只有一个 |
| **工厂方法** | 工厂创建对象，单例控制创建 |

## 总结

单例模式的核心要点：

- **适用场景**：全局唯一资源、配置管理、日志系统
- **推荐实现**：Meyers' Singleton（C++11 起线程安全）
- **核心风险**：线程安全、初始化顺序、滥用
- **替代方案**：依赖注入、依赖容器

> **记住**：单例模式是一把双刃剑——用得好可以提高代码组织性，用得不好会导致难以测试和维护的全局状态。
