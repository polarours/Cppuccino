# 策略模式 (Strategy Pattern)

## 什么是策略模式？

策略模式（Strategy Pattern）是一种行为型设计模式，它定义了一系列算法，将每个算法封装起来，并使它们可以互相替换。策略模式让算法的变化独立于使用算法的客户。

> **核心思想**：将可变的行为封装到策略类中，通过组合而非继承来实现行为的灵活切换。

## 为什么需要策略模式？

### 问题场景

假设我们有一个电商系统的折扣计算：

```cpp
// 问题：硬编码的折扣逻辑
class Order {
public:
    double calculateDiscount(const std::string& customerType) {
        if (customerType == "VIP") {
            return 0.2;  // 20% 折扣
        } else if (customerType == "Regular") {
            return 0.05;  // 5% 折扣
        } else if (customerType == "New") {
            return 0.1;  // 10% 折扣
        }
        return 0.0;
    }
};
```

**问题**：
1. 违反开闭原则：添加新折扣策略需要修改 Order 类
2. 算法耦合：折扣逻辑与订单逻辑混在一起
3. 难以测试：无法单独测试折扣算法

### 解决方案

使用策略模式：

```cpp
class DiscountStrategy {
public:
    virtual double calculate(double amount) const = 0;
    virtual ~DiscountStrategy() = default;
};

class VIPDiscount : public DiscountStrategy {
public:
    double calculate(double amount) const override {
        return amount * 0.2;
    }
};

class Order {
public:
    void setStrategy(std::unique_ptr<DiscountStrategy> strategy) {
        strategy_ = std::move(strategy);
    }
    
    double calculateDiscount(double amount) const {
        return strategy_->calculate(amount);
    }

private:
    std::unique_ptr<DiscountStrategy> strategy_;
};
```

**优点**：
1. 解耦：订单类不关心具体折扣算法
2. 可扩展：添加新策略只需新增类
3. 可测试：每个策略可以独立测试
4. 符合开闭原则：对扩展开放，对修改关闭

## 模式结构

```
                    ┌─────────────┐
                    │  Context    │
                    │  (订单)      │
                    └──────┬──────┘
                           │ composes
                           │
              ┌────────────▼────────────┐
              │      Strategy           │ ← 抽象策略接口
              │   (DiscountStrategy)    │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  VIPDiscount │ │ RegularDisc │ │  NewDiscount │ ← 具体策略
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义抽象策略

```cpp
#include <memory>
#include <string>
#include <functional>

// 抽象策略接口
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) const = 0;
    virtual std::string name() const = 0;
};
```

### 2. 实现具体策略

```cpp
// 冒泡排序策略
class BubbleSortStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) const override {
        for (size_t i = 0; i < data.size(); ++i) {
            for (size_t j = 0; j < data.size() - 1 - i; ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
    
    std::string name() const override {
        return "Bubble Sort";
    }
};

// 快速排序策略
class QuickSortStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) const override {
        quickSort(data, 0, data.size() - 1);
    }
    
    std::string name() const override {
        return "Quick Sort";
    }

private:
    void quickSort(std::vector<int>& data, int low, int high) const {
        if (low < high) {
            int pi = partition(data, low, high);
            quickSort(data, low, pi - 1);
            quickSort(data, pi + 1, high);
        }
    }
    
    int partition(std::vector<int>& data, int low, int high) const {
        int pivot = data[high];
        int i = low - 1;
        
        for (int j = low; j < high; ++j) {
            if (data[j] <= pivot) {
                ++i;
                std::swap(data[i], data[j]);
            }
        }
        std::swap(data[i + 1], data[high]);
        return i + 1;
    }
};

// 插入排序策略
class InsertionSortStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) const override {
        for (size_t i = 1; i < data.size(); ++i) {
            int key = data[i];
            int j = i - 1;
            
            while (j >= 0 && data[j] > key) {
                data[j + 1] = data[j];
                --j;
            }
            data[j + 1] = key;
        }
    }
    
    std::string name() const override {
        return "Insertion Sort";
    }
};
```

### 3. 实现上下文

```cpp
class Sorter {
public:
    explicit Sorter(std::unique_ptr<SortStrategy> strategy)
        : strategy_(std::move(strategy)) {}
    
    void setStrategy(std::unique_ptr<SortStrategy> strategy) {
        strategy_ = std::move(strategy);
    }
    
    void sort(std::vector<int>& data) const {
        strategy_->sort(data);
    }
    
    std::string getStrategyName() const {
        return strategy_->name();
    }

private:
    std::unique_ptr<SortStrategy> strategy_;
};
```

### 4. 客户端使用

```cpp
#include <iostream>
#include <vector>
#include <random>

int main() {
    std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
    
    // 使用不同的排序策略
    Sorter sorter1(std::make_unique<BubbleSortStrategy>());
    Sorter sorter2(std::make_unique<QuickSortStrategy>());
    Sorter sorter3(std::make_unique<InsertionSortStrategy>());
    
    std::vector<int> data1 = data;
    sorter1.sort(data1);
    std::cout << "Bubble Sort: ";
    for (int x : data1) std::cout << x << " ";
    std::cout << "\n";
    
    std::vector<int> data2 = data;
    sorter2.sort(data2);
    std::cout << "Quick Sort:  ";
    for (int x : data2) std::cout << x << " ";
    std::cout << "\n";
    
    std::vector<int> data3 = data;
    sorter3.sort(data3);
    std::cout << "Insertion:   ";
    for (int x : data3) std::cout << x << " ";
    std::cout << "\n";
    
    // 运行时切换策略
    sorter1.setStrategy(std::make_unique<QuickSortStrategy>());
    std::cout << "Switched to: " << sorter1.getStrategyName() << "\n";
    
    return 0;
}
```

## 现代 C++ 实现

### 使用 std::function

对于简单的策略，可以使用 std::function：

```cpp
#include <functional>
#include <algorithm>

class SorterModern {
public:
    using SortFunc = std::function<void(std::vector<int>&)>;
    
    void setStrategy(SortFunc sortFunc) {
        sortFunc_ = std::move(sortFunc);
    }
    
    void sort(std::vector<int>& data) const {
        sortFunc_(data);
    }

private:
    SortFunc sortFunc_;
};

// 使用 lambda
SorterModern sorter;
sorter.setStrategy([](std::vector<int>& data) {
    std::sort(data.begin(), data.end());
});
```

### 使用模板策略

```cpp
template<typename Strategy>
class SorterTemplate {
public:
    void sort(std::vector<int>& data) const {
        Strategy strategy;
        strategy.sort(data);
    }
};

// 编译期选择策略
using QuickSorter = SorterTemplate<QuickSortStrategy>;
using BubbleSorter = SorterTemplate<BubbleSortStrategy>;
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **状态模式** | 状态模式依赖当前状态切换行为；策略模式由客户端选择策略 |
| **工厂方法** | 工厂方法创建对象；策略模式封装算法 |
| **模板方法** | 模板方法在父类定义算法骨架；策略模式替换整个算法 |

## 最佳实践

### 1. 策略应该小而专一

```cpp
// 推荐：每个策略只做一件事
class CompressionStrategy {
public:
    virtual std::string compress(const std::string& data) const = 0;
};

class ZipCompression : public CompressionStrategy {
public:
    std::string compress(const std::string& data) const override {
        // 只处理 ZIP 压缩
    }
};

class GzipCompression : public CompressionStrategy {
public:
    std::string compress(const std::string& data) const override {
        // 只处理 GZIP 压缩
    }
};
```

### 2. 使用上下文管理策略生命周期

```cpp
class ImageProcessor {
public:
    void process(const std::string& image) {
        // 策略由上下文管理
        auto filter = createFilter();
        filter->apply(image);
    }

private:
    std::unique_ptr<FilterStrategy> createFilter() const {
        return std::make_unique<BlurFilter>();
    }
};
```

### 3. 考虑策略缓存

```cpp
class StrategyRegistry {
public:
    static StrategyRegistry& getInstance() {
        static StrategyRegistry instance;
        return instance;
    }
    
    void registerStrategy(const std::string& name, std::unique_ptr<Strategy> strategy) {
        strategies_[name] = std::move(strategy);
    }
    
    Strategy* getStrategy(const std::string& name) const {
        auto it = strategies_.find(name);
        return it != strategies_.end() ? it->second.get() : nullptr;
    }

private:
    std::map<std::string, std::unique_ptr<Strategy>> strategies_;
};
```

## 常见陷阱

### 陷阱 1：策略过于复杂

```cpp
// 不推荐：策略类包含过多逻辑
class DiscountStrategy {
public:
    double calculate(double amount) const override {
        // 复杂的多条件逻辑
        if (isVIP() && isHoliday() && isNewCustomer()) {
            return amount * 0.15;
        }
        // ...
    }
};

// 推荐：拆分成多个策略
class VIPHolidayDiscount : public DiscountStrategy { ... };
class NewCustomerDiscount : public DiscountStrategy { ... };
```

### 陷阱 2：忘记设置策略

```cpp
// 不推荐：未设置策略时行为不确定
class Context {
public:
    void execute() {
        strategy_->doSomething();  // 可能为空！
    }
    
private:
    Strategy* strategy_ = nullptr;  // 危险！
};

// 推荐：确保策略始终有效
class Context {
public:
    explicit Context(std::unique_ptr<Strategy> strategy)
        : strategy_(std::move(strategy)) {}
    
    void execute() {
        strategy_->doSomething();  // 保证非空
    }

private:
    std::unique_ptr<Strategy> strategy_;
};
```

### 陷阱 3：策略间状态共享

```cpp
// 不推荐：策略共享可变状态
class SharedState {
public:
    static int counter;
};

class StrategyA : public Strategy {
public:
    void execute() override {
        SharedState::counter++;  // 共享状态
    }
};

// 推荐：每个策略独立状态
class StrategyA : public Strategy {
public:
    void execute() override {
        int localCounter = 0;  // 独立状态
    }
};
```

## 总结

策略模式的核心要点：

- **适用场景**：多个算法需要动态切换、避免条件分支、算法需要独立测试
- **核心优点**：解耦、可扩展、符合开闭原则
- **实现要点**：使用智能指针、确保策略有效性、避免共享状态
- **常见陷阱**：策略过于复杂、未设置策略、共享状态

> **记住**：策略模式的关键是"行为抽象"——将变化的行为封装到独立的类中，让算法可以独立于使用它的客户端而变化。
