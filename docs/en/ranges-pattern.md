# Ranges 模式与深度理解 (Ranges Pattern)

## 什么是 Ranges？

C++20 Ranges 是 `<ranges>` 库提供的声明式数据处理抽象。核心思想：**数据流**——用管道操作符 `|` 把过滤、变换、截取等步骤串联起来，形成一条"数据流水线"，而不需要手动写循环和临时容器。

> **核心思想**：惰性求值（lazy evaluation）——每步只产生一个 view，不实际遍历，直到你消费它。

## 与算法的对比

| | std::algorithm | std::ranges |
|---|---|---|
| 调用方式 | `std::sort(v.begin(), v.end())` | `v | std::views::sort` |
| 临时容器 | 需要 `copy_if` 前手动创建 | view 延迟消费，无额外分配 |
| 组合性 | 多个算法需多次传递迭代器 | 链式管道，表达力更强 |

## Range 适配器（View Adapters）

Ranges 的核心是各种"view"适配器：

| View | 作用 | 示例 |
|---|---|---|
| `filter(p)` | 过滤元素 | `nums \| std::views::filter([](int n){ return n % 2 == 0; })` |
| `transform(fn)` | 映射元素 | `names \| std::views::transform([](auto& s){ return s.size(); })` |
| `take(n)` | 取前 N 个 | `values \| std::views::take(5)` |
| `skip(n)` | 跳过前 N 个 | `items \| std::views::skip(2)` |
| `reverse()` | 反向 | `seq \| std::views::reverse` |
| `join()` | 展平嵌套序列 | `matrix \| std::views::join` |
| `keys()` / `values()` | 解包 pair 序列 | `map \| std::views::keys` |

## 惰性求值与管道

```cpp
#include <ranges>
#include <vector>
#include <algorithm>
#include <iostream>

int main() {
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 链式管道：filter → transform → take
    auto result = nums
        | std::views::filter([](int n){ return n % 2 == 0; })  // [2,4,6,8,10]
        | std::views::transform([](int n){ return n * n; })    // [4,16,36,64,100]
        | std::views::take(3);                                  // [4,16,36]

    for (int n : result) std::cout << n << " ";  // 4 16 36
}
```

注意：`result` 本身不是一个 `vector`，而是一个 view —— 只有在 `for` 遍历时才真正触发计算。

## Range 概念体系

| 概念 | 要求 | 含义 |
|---|---|---|
| `std::ranges::range` | `begin()`/`end()` | 任何可迭代序列 |
| `std::ranges::input_range` | `range` + `input_iterator` | 单向读取 |
| `std::ranges::forward_range` | `range` + `forward_iterator` | 可反复扫描 |
| `std::ranges::bidirectional_range` | `forward_range` + 双向 | 可前后遍历 |
| `std::ranges::random_access_range` | `bidirectional` + 随机访问 | 支持 `[]` |
| `std::ranges::sized_range` | `range` + `size()` | 已知大小 |
| `std::ranges::viewable_range` | 能被 `std::views::all` 包装 | 生命周期安全的 range |

## 自定义 View

实现一个自定义 view 需要两个组件：`view base`（迭代器类）和 `adapter`（持有 base）。

```cpp
#include <ranges>

class EvenView : public std::ranges::view_base {
    std::vector<int>& data_;
public:
    EvenView(std::vector<int>& d) : data_(d) {}

    struct iterator {
        std::vector<int>::iterator cur;
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using iterator_category = std::input_iterator_tag;

        iterator& operator++() {
            while (++cur != data_.end() && *cur % 2 != 0) {}
            return *this;
        }
        int operator*() const { return *cur; }
        bool operator==(const iterator&) const = default;
    };

    iterator begin() {
        iterator it{data_.begin()};
        ++it;  // 跳过第一个奇数
        return it;
    }
    iterator end() { return {data_.end()}; }
};
```

## 最佳实践

1. **优先用 range-based 调用**：`v | std::views::filter(...)` 比显式迭代器更清晰
2. **注意 view 的生命周期**：view 不持有数据，若底层容器销毁后继续迭代会 UB
3. **用 `std::ranges::iota_view` 替代手动循环**：`for (int n : std::ranges::iota_view(1, 11))` 生成 1..10
4. **`take_while` / `drop_while` 比手动 break 更可读**
5. **`std::ranges::sort` 需要 random-access_range**；普通容器可以，但 `std::list` 不行

## 总结

Ranges 是 C++ 从"命令式遍历"走向"声明式流水线"的关键一步。配合概念系统，编译器能给出更有意义的错误信息。理解 view 的惰性语义和生命周期，是写出高效且安全的 range 代码的前提。
