# 策略模式 (Strategy Pattern)

## 什么是策略模式？

策略模式（Strategy Pattern）定义一族算法，把每个算法**封装**起来，并使它们可以**互相替换**。策略让算法的变化独立于使用算法的客户。

> **核心思想**：把"会变的行为"抽到独立类族，Context 通过接口持有当前策略。

## 适用场景

- 多种排序/压缩/加密算法可选
- 业务规则多变（折扣、税率、运费计算）
- 同一任务的多种实现（同步/异步、内存/磁盘）

## 核心概念

- **Strategy 接口**：声明算法的统一接口
- **Concrete Strategy**：实现具体算法
- **Context**：持有 `unique_ptr<Strategy>`，转发请求到当前策略
- **可替换性**：运行时通过 `setStrategy` 切换

## 与状态模式的对比

| | Strategy | State |
|---|---|---|
| 切换方 | 客户端显式选择 | 状态对象内部迁移 |
| 策略数量 | 平等、可任选 | 通常有迁移图 |
| 意图 | "怎么做" | "现在是什么状态" |

## 代码实现

```cpp
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

template <typename T>
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<T>& data) = 0;
    virtual std::string name() const = 0;
};

template <typename T>
class BubbleSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        for (size_t i = 0; i < data.size(); ++i) {
            for (size_t j = 0; j + 1 < data.size() - i; ++j) {
                if (data[j] > data[j + 1]) std::swap(data[j], data[j + 1]);
            }
        }
    }
    std::string name() const override { return "BubbleSort"; }
};

template <typename T>
class QuickSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        if (data.size() <= 1) return;
        quick(data, 0, static_cast<int>(data.size()) - 1);
    }
    std::string name() const override { return "QuickSort"; }
private:
    void quick(std::vector<T>& a, int lo, int hi) {
        if (lo >= hi) return;
        T pivot = a[hi]; int i = lo;
        for (int j = lo; j < hi; ++j) if (a[j] < pivot) std::swap(a[i++], a[j]);
        std::swap(a[i], a[hi]);
        quick(a, lo, i - 1); quick(a, i + 1, hi);
    }
};

template <typename T>
class Sorter {
public:
    explicit Sorter(std::shared_ptr<SortStrategy<T>> s) : strategy_(std::move(s)) {}
    void setStrategy(std::shared_ptr<SortStrategy<T>> s) { strategy_ = std::move(s); }
    void sort(std::vector<T>& data) { strategy_->sort(data); }
    const std::string& currentStrategyName() const { return strategy_->name(); }
private:
    std::shared_ptr<SortStrategy<T>> strategy_;
};
```

完整可运行示例见 [strategy-pattern 项目](../../cpp-mastery-roadmap/09-stage9/strategy-pattern)。

## 使用示例

```cpp
std::vector<int> v{5, 1, 4, 2, 3};
Sorter<int> sorter(std::make_shared<QuickSort<int>>());
sorter.sort(v);
assert(sorter.currentStrategyName() == "QuickSort");

sorter.setStrategy(std::make_shared<BubbleSort<int>>());
sorter.sort(v);
assert(sorter.currentStrategyName() == "BubbleSort");
```

## 关键陷阱

1. **空策略**：`Sorter` 持有 `nullptr` 时 `sort` 会崩——构造时必须传入默认策略
2. **策略对象有状态**：把 QuickSort 改成"原地 partition + 缓存"需要警惕"上一个排序残留状态"
3. **多策略组合**：当一种 Context 需要同时用多种策略时（折扣 + 运费 + 税），考虑用**装饰器**或**责任链**而非一个 Context 持多个 Strategy
4. **性能开销**：虚函数调用 + 间接寻址——在热路径上可考虑 CRTP 静态多态

## 最佳实践

1. **用 `unique_ptr` 而非 `shared_ptr`**：策略通常是无状态的，`unique_ptr` 表达意图更清晰；本项目用 `shared_ptr` 是因为想和模板参数统一
2. **默认策略**：Context 构造时接受一个默认策略，避免空指针
3. **策略注册表**：用 `std::function` 也能实现轻量策略（见 strategy-adapter 项目）

## 总结

策略模式把"会变的行为"从 Context 中抽离，让新增算法只需一个新类；它和工厂、模板方法、装饰器是"行为可变"这一主题下的不同表达方式。
