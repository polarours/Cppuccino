# 规范模式 (Specification Pattern)

## 什么是规范模式？

规范模式（Specification Pattern）是一种行为型设计模式，它将业务规则封装成可重用的规范对象，使业务规则的验证和组合更加灵活和可维护。

> **核心思想**：将"是否符合某个条件"的逻辑封装成独立的规范对象，支持组合和复用。

## 为什么需要规范模式？

### 问题场景

假设我们有一个电商系统，需要验证订单：

```cpp
// 问题：验证逻辑分散且难以复用
class Order {
public:
    bool isValid() const {
        return !items_.empty() 
            && total_ > 0 
            && customer_.hasValidAddress()
            && customer_.hasValidPayment();
    }
    
    bool canShip() const {
        return isValid() 
            && total_ >= 100 
            && customer_.isVip();
    }

private:
    std::vector<Item> items_;
    double total_ = 0;
    Customer customer_;
};
```

**问题**：
1. 验证逻辑硬编码在类中
2. 规则难以复用
3. 组合复杂规则困难

### 解决方案

使用规范模式：

```cpp
// 规范接口
template<typename T>
class Specification {
public:
    virtual ~Specification() = default;
    virtual bool isSatisfiedBy(const T& item) const = 0;
    virtual std::unique_ptr<Specification<T>> and_(
        const std::unique_ptr<Specification<T>>& other) const = 0;
    virtual std::unique_ptr<Specification<T>> or_(
        const std::unique_ptr<Specification<T>>& other) const = 0;
    virtual std::unique_ptr<Specification<T>> not_() const = 0;
};

// 具体规范
class TotalGreaterThanSpecification : public Specification<Order> {
public:
    explicit TotalGreaterThanSpecification(double amount) : amount_(amount) {}
    
    bool isSatisfiedBy(const Order& order) const override {
        return order.getTotal() > amount_;
    }
    
    std::unique_ptr<Specification<Order>> and_(
        const std::unique_ptr<Specification<Order>>& other) const override {
        return std::make_unique<AndSpecification<Order>>(
            std::unique_ptr<Specification<Order>>(this), other);
    }
    
    // ... or_ 和 not_ 实现
private:
    double amount_;
};

// 使用
auto spec = std::make_unique<TotalGreaterThanSpecification>(100)
    ->and_(std::make_unique<CustomerIsVipSpecification>());
    
bool result = spec->isSatisfiedBy(order);
```

**优点**：
1. 规则可复用
2. 易于组合
3. 符合开闭原则

## 模式结构

```
                    ┌─────────────┐
                    │Specification│
                    │  (规范)      │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ AndSpec     │ │  OrSpec     │ │ NotSpec     │ ← 组合规范
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │    ConcreteSpecification│ ← 具体规范
              │   (TotalGreaterThan)    │
              └────────────┬────────────┘
                           │
                    ┌────────▼────────┐
                    │     Target      │ ← 被验证对象
                    └─────────────────┘
```

## 代码实现

### 1. 定义规范接口

```cpp
#include <memory>
#include <functional>
#include <iostream>
#include <vector>
#include <string>

template<typename T>
class Specification {
public:
    virtual ~Specification() = default;
    virtual bool isSatisfiedBy(const T& item) const = 0;
    
    // 组合操作
    std::unique_ptr<Specification<T>> and_(
        const std::unique_ptr<Specification<T>>& other) const {
        return std::make_unique<AndSpecification<T>>(shared_from_this(), other);
    }
    
    std::unique_ptr<Specification<T>> or_(
        const std::unique_ptr<Specification<T>>& other) const {
        return std::make_unique<OrSpecification<T>>(shared_from_this(), other);
    }
    
    std::unique_ptr<Specification<T>> not_() const {
        return std::make_unique<NotSpecification<T>>(shared_from_this());
    }

protected:
    virtual std::shared_ptr<const Specification<T>> getSelf() const = 0;
};
```

### 2. 实现组合规范

```cpp
template<typename T>
class AndSpecification : public Specification<T>, 
                          public std::enable_shared_from_this<AndSpecification<T>> {
public:
    AndSpecification(std::shared_ptr<const Specification<T>> left,
                     std::unique_ptr<Specification<T>> right)
        : left_(left), right_(std::move(right)) {}
    
    bool isSatisfiedBy(const T& item) const override {
        bool result = left_->isSatisfiedBy(item) && right_->isSatisfiedBy(item);
        std::cout << "And: " << item << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<T>> getSelf() const override {
        return shared_from_this();
    }

private:
    std::shared_ptr<const Specification<T>> left_;
    std::unique_ptr<Specification<T>> right_;
};

template<typename T>
class OrSpecification : public Specification<T>,
                         public std::enable_shared_from_this<OrSpecification<T>> {
public:
    OrSpecification(std::shared_ptr<const Specification<T>> left,
                    std::unique_ptr<Specification<T>> right)
        : left_(left), right_(std::move(right)) {}
    
    bool isSatisfiedBy(const T& item) const override {
        bool result = left_->isSatisfiedBy(item) || right_->isSatisfiedBy(item);
        std::cout << "Or: " << item << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<T>> getSelf() const override {
        return shared_from_this();
    }

private:
    std::shared_ptr<const Specification<T>> left_;
    std::unique_ptr<Specification<T>> right_;
};

template<typename T>
class NotSpecification : public Specification<T>,
                          public std::enable_shared_from_this<NotSpecification<T>> {
public:
    explicit NotSpecification(std::shared_ptr<const Specification<T>> spec)
        : spec_(spec) {}
    
    bool isSatisfiedBy(const T& item) const override {
        bool result = !spec_->isSatisfiedBy(item);
        std::cout << "Not: " << item << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<T>> getSelf() const override {
        return shared_from_this();
    }

private:
    std::shared_ptr<const Specification<T>> spec_;
};
```

### 3. 实现具体规范

```cpp
// 订单总金额的规范
class TotalGreaterThanSpecification : public Specification<int>,
                                      public std::enable_shared_from_this<TotalGreaterThanSpecification> {
public:
    explicit TotalGreaterThanSpecification(int amount) : amount_(amount) {}
    
    bool isSatisfiedBy(const int& total) const override {
        bool result = total > amount_;
        std::cout << "Total > " << amount_ << ": " << total 
                  << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<int>> getSelf() const override {
        return shared_from_this();
    }

private:
    int amount_;
};

// 订单数量的规范
class ItemCountSpecification : public Specification<int>,
                                public std::enable_shared_from_this<ItemCountSpecification> {
public:
    explicit ItemCountSpecification(int minCount, int maxCount)
        : minCount_(minCount), maxCount_(maxCount) {}
    
    bool isSatisfiedBy(const int& count) const override {
        bool result = count >= minCount_ && count <= maxCount_;
        std::cout << "Count [" << minCount_ << "," << maxCount_ << "]: " 
                  << count << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<int>> getSelf() const override {
        return shared_from_this();
    }

private:
    int minCount_;
    int maxCount_;
};

// 黑名单规范
class NotInBlacklistSpecification : public Specification<std::string>,
                                     public std::enable_shared_from_this<NotInBlacklistSpecification> {
public:
    explicit NotInBlacklistSpecification(const std::vector<std::string>& blacklist)
        : blacklist_(blacklist) {}
    
    bool isSatisfiedBy(const std::string& customer) const override {
        bool result = std::find(blacklist_.begin(), blacklist_.end(), customer) 
                      == blacklist_.end();
        std::cout << "Customer '" << customer << "' not in blacklist: "
                  << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<std::string>> getSelf() const override {
        return shared_from_this();
    }

private:
    std::vector<std::string> blacklist_;
};
```

### 4. 客户端使用

```cpp
int main() {
    // 创建规范
    auto totalSpec = std::make_shared<TotalGreaterThanSpecification>(100);
    auto countSpec = std::make_shared<ItemCountSpecification>(1, 10);
    auto blacklistSpec = std::make_shared<NotInBlacklistSpecification>(
        std::vector<std::string>{"bad_customer_1", "bad_customer_2"});
    
    // 组合规范
    auto complexSpec = totalSpec->and_(countSpec->or_(blacklistSpec));
    
    // 验证
    std::cout << "=== Testing ===\n";
    complexSpec->isSatisfiedBy(150);
    
    // 简单规范组合
    auto simpleSpec = totalSpec->and_(blacklistSpec);
    std::cout << "\n=== Simple Test ===\n";
    simpleSpec->isSatisfiedBy(50);
    
    return 0;
}
```

## 现代 C++ 实现

### 使用 lambda 简化

```cpp
class LambdaSpecification {
public:
    using Predicate = std::function<bool(const int&)>;
    
    LambdaSpecification(Predicate pred) : pred_(std::move(pred)) {}
    
    bool isSatisfiedBy(const int& item) const {
        return pred_(item);
    }
    
    std::unique_ptr<LambdaSpecification> and_(
        const std::unique_ptr<LambdaSpecification>& other) const {
        auto pred = [this, other](const int& item) {
            return pred_(item) && other->pred_(item);
        };
        return std::make_unique<LambdaSpecification>(std::move(pred));
    }
    
    Predicate pred_;
};
```

### 使用模板简化组合

```cpp
template<typename T>
class CompositeSpecification : public Specification<T> {
public:
    CompositeSpecification(std::unique_ptr<Specification<T>> left,
                           std::unique_ptr<Specification<T>> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    
protected:
    std::unique_ptr<Specification<T>> left_;
    std::unique_ptr<Specification<T>> right_;
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **策略模式** | 策略模式选择算法；规范模式验证条件 |
| **命令模式** | 命令模式封装请求；规范模式封装条件 |
| **责任链** | 责任链顺序处理；规范模式组合验证 |

## 最佳实践

### 1. 使用工厂创建规范

```cpp
class SpecificationFactory {
public:
    static std::shared_ptr<Specification<int>> createTotalSpecification(int amount) {
        return std::make_shared<TotalGreaterThanSpecification>(amount);
    }
    
    static std::shared_ptr<Specification<int>> createCountSpecification(
        int min, int max) {
        return std::make_shared<ItemCountSpecification>(min, max);
    }
};
```

### 2. 支持缓存结果

```cpp
class CachedSpecification : public Specification<T> {
public:
    bool isSatisfiedBy(const T& item) const override {
        auto it = cache_.find(item);
        if (it != cache_.end()) {
            return it->second;
        }
        bool result = spec_->isSatisfiedBy(item);
        cache_[item] = result;
        return result;
    }

private:
    mutable std::map<T, bool> cache_;
    std::unique_ptr<Specification<T>> spec_;
};
```

### 3. 支持调试输出

```cpp
class DebugSpecification : public Specification<T> {
public:
    bool isSatisfiedBy(const T& item) const override {
        std::cout << "[Debug] Checking: " << item << "\n";
        bool result = spec_->isSatisfiedBy(item);
        std::cout << "[Debug] Result: " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }
};
```

## 常见陷阱

### 陷阱 1：规范过于复杂

```cpp
// 不推荐：单个规范包含太多逻辑
class BadSpecification : public Specification<Order> {
public:
    bool isSatisfiedBy(const Order& order) const override {
        // 太多逻辑，难以测试和维护
        if (order.getTotal() > 100 && order.getCount() > 0 
            && order.getCustomer().isVip() && ...) {
            return true;
        }
        return false;
    }
};

// 推荐：拆分成多个小规范
class TotalSpecification { ... };
class CountSpecification { ... };
class CustomerSpecification { ... };
```

### 陷阱 2：循环依赖

```cpp
// 不推荐：规范互相依赖
class SpecA : public Specification<T> {
    std::shared_ptr<SpecB> specB_;  // 依赖 SpecB
};

class SpecB : public Specification<T> {
    std::shared_ptr<SpecA> specA_;  // 依赖 SpecA - 循环！
};
```

### 陷阱 3：性能问题

```cpp
// 不推荐：每次验证都重新创建规范
bool check(const Order& order) {
    auto spec = std::make_unique<ComplexSpecification>();  // 每次创建
    return spec->isSatisfiedBy(order);
}

// 推荐：复用规范对象
class OrderChecker {
public:
    OrderChecker() : spec_(std::make_unique<ComplexSpecification>()) {}
    
    bool check(const Order& order) {
        return spec_->isSatisfiedBy(order);
    }

private:
    std::unique_ptr<Specification<Order>> spec_;
};
```

## 总结

规范模式的核心要点：

- **适用场景**：需要复用验证逻辑、组合复杂规则、业务规则频繁变化
- **核心优点**：可复用、易组合、符合开闭原则
- **实现要点**：使用智能指针、支持组合操作、缓存结果
- **常见陷阱**：规范过于复杂、循环依赖、性能问题

> **记住**：规范模式的关键是"规则封装"——将验证逻辑封装成独立的规范对象，使业务规则可以复用和组合。
