# 观察者模式 (Observer Pattern)

## 什么是观察者模式？

观察者模式（Observer Pattern）是一种行为型设计模式，它定义了一种一对多的依赖关系，让多个观察者对象同时监听某一个主题对象。当主题对象状态发生变化时，所有依赖于它的观察者都会得到通知并自动更新。

> **核心思想**：定义对象间的一种一对多的依赖关系，当一个对象的状态发生改变时，所有依赖于它的对象都得到通知并被自动更新。

## 为什么需要观察者模式？

### 问题场景

假设我们有一个股票价格系统：

```cpp
// 问题：硬编码的更新逻辑
class Stock {
public:
    void setPrice(double price) {
        price_ = price;
        notifyTradingApp();    // 特定耦合
        notifyNewsService();   // 特定耦合
        notifyAlertSystem();   // 特定耦合
    }
    
private:
    double price_;
    void notifyTradingApp() { /* ... */ }
    void notifyNewsService() { /* ... */ }
    void notifyAlertSystem() { /* ... */ }
};
```

**问题**：
1. 股票类与所有观察者类紧密耦合
2. 添加新观察者需要修改 Stock 类
3. 违反开闭原则

### 解决方案

使用观察者模式：

```cpp
class Stock {
public:
    void attach(Observer& observer) {
        observers_.push_back(&observer);
    }
    
    void setPrice(double price) {
        price_ = price;
        notifyAll();
    }
    
private:
    std::vector<Observer*> observers_;
    double price_;
    
    void notifyAll() {
        for (auto* observer : observers_) {
            observer->update(price_);
        }
    }
};
```

**优点**：
1. 解耦：主题不知道具体观察者
2. 可扩展：添加新观察者无需修改主题
3. 灵活：运行时可以动态添加/移除观察者

## 模式结构

```
                    ┌─────────────┐
                    │  Subject    │
                    │  (主题)      │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Stock      │ │  Bond       │ │  Commodity  │ ← 具体主题
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │       Observer          │ ← 抽象观察者
              │    (interface)          │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ TradingApp  │ │ NewsService │ │ AlertSystem │ ← 具体观察者
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义抽象主题

```cpp
#include <vector>
#include <memory>
#include <functional>

// 观察者接口
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(double price) = 0;
};

// 主题接口
class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(Observer* observer) = 0;
    virtual void detach(Observer* observer) = 0;
    virtual void notify() = 0;
};
```

### 2. 实现具体主题

```cpp
class Stock : public Subject {
public:
    void attach(Observer* observer) override {
        observers_.push_back(observer);
    }
    
    void detach(Observer* observer) override {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end()
        );
    }
    
    void notify() override {
        for (auto* observer : observers_) {
            observer->update(price_);
        }
    }
    
    void setPrice(double price) {
        if (price_ != price) {
            price_ = price;
            notify();
        }
    }

private:
    std::vector<Observer*> observers_;
    double price_ = 0.0;
};
```

### 3. 实现具体观察者

```cpp
class TradingApp : public Observer {
public:
    void update(double price) override {
        std::cout << "[TradingApp] Stock price updated: $" << price << "\n";
        // 执行交易逻辑
    }
};

class NewsService : public Observer {
public:
    void update(double price) override {
        std::cout << "[NewsService] Breaking news: Stock at $" << price << "\n";
        // 发布新闻
    }
};

class AlertSystem : public Observer {
public:
    explicit AlertSystem(double threshold) : threshold_(threshold) {}
    
    void update(double price) override {
        if (price > threshold_) {
            std::cout << "[AlertSystem] Price exceeded threshold! $" << price << "\n";
        }
    }

private:
    double threshold_;
};
```

### 4. 客户端使用

```cpp
int main() {
    Stock stock;
    
    TradingApp trading;
    NewsService news;
    AlertSystem alert(100.0);
    
    // 注册观察者
    stock.attach(&trading);
    stock.attach(&news);
    stock.attach(&alert);
    
    // 价格变动，所有观察者收到通知
    stock.setPrice(95.0);
    stock.setPrice(105.0);
    
    return 0;
}
```

## 现代 C++ 实现

### 使用 std::function 的观察者

```cpp
#include <functional>
#include <map>

class StockModern {
public:
    using Callback = std::function<void(double)>;
    
    void subscribe(const std::string& name, Callback callback) {
        callbacks_[name] = std::move(callback);
    }
    
    void unsubscribe(const std::string& name) {
        callbacks_.erase(name);
    }
    
    void setPrice(double price) {
        price_ = price;
        for (const auto& [name, callback] : callbacks_) {
            callback(price);
        }
    }

private:
    std::map<std::string, Callback> callbacks_;
    double price_ = 0.0;
};

// 使用
StockModern stock;
stock.subscribe("Trading", [](double price) {
    std::cout << "Trading: $" << price << "\n";
});
stock.subscribe("Alert", [](double price) {
    if (price > 100) std::cout << "Alert: High price!\n";
});
```

### 使用智能指针的观察者

```cpp
class SubjectSmart {
public:
    using ObserverPtr = std::shared_ptr<Observer>;
    
    void attach(ObserverPtr observer) {
        observers_.push_back(std::move(observer));
    }
    
    void notify() {
        for (auto& obs : observers_) {
            obs->update(price_);
        }
    }
    
    // 自动移除已删除的观察者
    void garbageCollect() {
        observers_.erase(
            std::remove_if(observers_.begin(), observers_.end(),
                [](const ObserverPtr& obs) { return !obs; }),
            observers_.end()
        );
    }

private:
    std::vector<ObserverPtr> observers_;
    double price_ = 0.0;
};
```

## 变体与扩展

### 变体 1：Push vs Pull 模式

**Push 模式**（主题推送数据）：
```cpp
// 主题知道观察者需要什么
virtual void update(double price, double volume) = 0;
```

**Pull 模式**（观察者拉取数据）：
```cpp
// 观察者自己获取数据
virtual void update() = 0;
// 观察者调用主题获取数据
double price = subject->getPrice();
```

### 变体 2：事件总线

```cpp
class EventBus {
public:
    template<typename Event>
    void subscribe(std::function<void(const Event&)> handler) {
        handlers_[typeid(Event).name()].push_back(handler);
    }
    
    template<typename Event>
    void publish(const Event& event) {
        auto it = handlers_.find(typeid(Event).name());
        if (it != handlers_.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

private:
    std::map<std::string, std::vector<std::function<void(const void*)>>> handlers_;
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **发布-订阅** | 发布-订阅通过事件总线解耦，观察者模式直接通知 |
| **策略模式** | 策略模式选择算法，观察者模式分发通知 |
| **观察者 vs 监听器** | 监听器是观察者的特例，通常用于 UI 事件 |

## 最佳实践

### 1. 使用弱引用避免循环依赖

```cpp
class WeakObserver {
public:
    std::weak_ptr<Observer> observer_;
    
    void update(double price) {
        if (auto obs = observer_.lock()) {
            obs->onUpdate(price);
        }
    }
};
```

### 2. 考虑使用信号槽机制

```cpp
#include <QObject>  // Qt 示例

class Stock : public QObject {
    Q_OBJECT
    Q_PROPERTY(double price READ price WRITE setPrice NOTIFY priceChanged)
    
public:
    void setPrice(double price) {
        if (price_ != price) {
            price_ = price;
            emit priceChanged(price);
        }
    }
    
signals:
    void priceChanged(double price);
};
```

### 3. 注意线程安全

```cpp
class ThreadSafeSubject {
public:
    void attach(Observer* observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.push_back(observer);
    }
    
    void notify() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* obs : observers_) {
            obs->update(price_);
        }
    }

private:
    std::vector<Observer*> observers_;
    double price_ = 0.0;
    std::mutex mutex_;
};
```

## 常见陷阱

### 陷阱 1：观察者生命周期

```cpp
// 危险：观察者被删除后主题仍持有悬垂指针
Subject* subject = new Subject();
Observer* observer = new Observer();
subject->attach(observer);
delete observer;  // subject 持有悬垂指针！
```

**解决方案**：使用智能指针或弱引用

### 陷阱 2：递归通知

```cpp
// 危险：观察者通知另一个观察者，导致无限递归
void ObserverA::update(double price) {
    subject_->setPrice(price * 2);  // 触发另一个通知
}
```

**解决方案**：使用标志位或快照机制

### 陷阱 3：通知期间修改观察者列表

```cpp
// 危险：在通知过程中添加/移除观察者
void notify() {
    for (auto* obs : observers_) {
        obs->update(price_);  // 可能修改 observers_
    }
}
```

**解决方案**：使用快照或延迟删除

## 总结

观察者模式的核心要点：

- **适用场景**：一对多的通知关系、事件驱动系统、GUI 编程
- **核心优点**：解耦、可扩展、符合开闭原则
- **实现要点**：注意生命周期、线程安全、避免递归
- **现代 C++**：使用 std::function、智能指针、信号槽

> **记住**：观察者模式的关键是"松耦合"——主题只关心通知，不关心谁在监听。
