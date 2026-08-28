# 服务定位器模式 (Service Locator Pattern)

## 什么是服务定位器模式？

服务定位器模式（Service Locator Pattern）是一种注册中心模式：把一组全局可用的服务实例集中注册到一个对象里，业务代码按名字查表取用，而不必在每个使用点都显式注入依赖。

> **核心思想**：一个中心化的查找表 `registerService(name, instance) / getService<T>(name)`，解耦"谁需要"和"谁提供"。

## 与依赖注入（DI）的关系

| | Service Locator | 依赖注入 |
|---|---|---|
| 服务来源 | 调用方主动 `locator.get<X>()` | 构造时由容器/外部传入 |
| 依赖可见性 | 隐藏（藏在 locator 内部） | 显式（构造参数列表） |
| 测试性 | 较弱（需全局替换 locator） | 较强（直接传入 mock） |
| 适用场景 | 框架/插件化系统、需要运行时注册 | 应用层业务对象 |

二者并非互斥：一个应用常常同时使用——DI 注入"业务服务"、Service Locator 暴露"横切能力"（日志、配置、插件）。

## 核心概念

- **注册**：`registerService(name, shared_ptr<void>)`，按字符串名字登记
- **类型化获取**：`getService<T>(name)` 用 `static_pointer_cast<T>` 还原类型
- **生命周期**：`shared_ptr` 持有，locator 不负责构造
- **可选存在**：`hasService(name)` 允许运行时探测

## 问题场景

```cpp
// 问题：业务代码直接 new 具体服务，难以替换实现
class ReportGenerator {
    PdfExporter exporter;   // 硬编码具体类型
};
```

### 解决方案

```cpp
ReportGenerator gen{locator.getService<IExporter>("exporter")};
```

## 代码实现

```cpp
#include <map>
#include <memory>
#include <string>

class ServiceLocator {
public:
    void registerService(const std::string& name, std::shared_ptr<void> service) {
        services_[name] = std::move(service);
    }

    std::shared_ptr<void> getRaw(const std::string& name) const {
        auto it = services_.find(name);
        return (it != services_.end()) ? it->second : nullptr;
    }

    template <typename T>
    std::shared_ptr<T> getService(const std::string& name) const {
        return std::static_pointer_cast<T>(getRaw(name));
    }

    bool hasService(const std::string& name) const {
        return services_.find(name) != services_.end();
    }

    void removeService(const std::string& name) { services_.erase(name); }
    std::size_t serviceCount() const { return services_.size(); }

private:
    std::map<std::string, std::shared_ptr<void>> services_;
};
```

完整可运行示例见 [service-locator 项目](../../cpp-mastery-roadmap/30-stage30/service-locator)。

## 使用示例

```cpp
struct ILogger { virtual ~ILogger() = default; virtual void log(const std::string&) = 0; };
struct ConsoleLogger : ILogger {
    void log(const std::string& m) override { /* ... */ }
};

ServiceLocator locator;
locator.registerService("logger", std::make_shared<ConsoleLogger>());

auto logger = locator.getService<ILogger>("logger");
logger->log("hello");
```

## 最佳实践

1. **避免全局单例**：尽量把 locator 实例作为参数显式传递，便于测试
2. **类型不匹配会静默失败**：`getService<T>` 的 `static_pointer_cast` 不会做运行时类型检查，调用方要保证名字+类型组合正确
3. **按"能力"取，按"实例"注入**：把多实例对象（数据库连接池槽位）放进 locator 会引起竞态——单例特性才是它的舒适区
4. **解构顺序**：locator 持有 `shared_ptr`，被 locator 持有的服务不应反向持有 locator，否则循环引用

## 总结

Service Locator 提供"运行时查表"的能力，是插件系统、跨切关注点（logging/config/feature flag）的常见落点；但应控制使用面——核心业务依赖优先用显式 DI，否则可测试性会迅速恶化。
