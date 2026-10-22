# Strategy Pattern

## What Is the Strategy Pattern?

The strategy pattern defines a family of algorithms, **encapsulates** each one,
and makes them **interchangeable**. Strategy lets the algorithm vary
independently from the clients that use it.

> **Core idea**: extract the "behavior that changes" into a family of standalone
> classes; the Context holds the current strategy through an interface.

## When It Fits

- Multiple sorting/compression/encryption algorithms to choose from
- Business rules that change often (discounts, tax rates, shipping costs)
- Several implementations of the same task (sync/async, memory/disk)

## Core Concepts

- **Strategy interface**: declares the uniform algorithm interface
- **Concrete strategy**: implements one algorithm
- **Context**: holds a `shared_ptr<Strategy>` and forwards requests to the
  current strategy
- **Interchangeability**: swap at runtime via `setStrategy`

## Strategy vs State

| | Strategy | State |
|---|---|---|
| Who switches | The client chooses explicitly | Transitions happen inside the state objects |
| Relationship among variants | Equal, any may be chosen | Usually a transition graph |
| Intent | "how to do it" | "what state am I in now" |

## Implementation

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
    // returns by value: strategy_->name() returns a temporary — never bind it to a const&
    std::string getStrategyName() const {
        return strategy_ ? strategy_->name() : "None";
    }
private:
    std::shared_ptr<SortStrategy<T>> strategy_;
};
```

For the full runnable example, see the
[strategy-pattern project](../../cpp-mastery-roadmap/09-stage9/strategy-pattern).

## Usage Example

```cpp
#include <cassert>

std::vector<int> v{5, 1, 4, 2, 3};
Sorter<int> sorter(std::make_shared<QuickSort<int>>());
sorter.sort(v);
assert(sorter.getStrategyName() == "QuickSort");

sorter.setStrategy(std::make_shared<BubbleSort<int>>());
sorter.sort(v);
assert(sorter.getStrategyName() == "BubbleSort");
```

## Key Traps

1. **Null strategy**: `Sorter` holding `nullptr` crashes on `sort` — always
   pass a default strategy at construction
2. **Stateful strategies**: if QuickSort were rewritten to "in-place partition +
   cache", watch for residue from the previous sort
3. **Multiple strategies at once**: when a Context needs several strategies
   simultaneously (discount + shipping + tax), prefer a **decorator** or
   **chain of responsibility** over one Context holding many strategies
4. **Performance cost**: virtual call + indirection — consider CRTP static
   polymorphism on hot paths

## Best Practices

1. **Prefer `unique_ptr` over `shared_ptr`**: strategies are usually stateless
   and `unique_ptr` states intent better; this project uses `shared_ptr` to stay
   consistent with the template parameter
2. **Default strategy**: the Context accepts one at construction to avoid null
   pointers
3. **Strategy registry**: `std::function` also works for lightweight strategies
   (see the strategy-adapter project)

## Example

A runnable version of this topic:

- [strategy-pattern-demo.cpp](../../examples/strategy-pattern-demo.cpp)

## Summary

Strategy extracts the "behavior that varies" out of the Context so adding an
algorithm means adding one class; it, Factory, Template Method, and Decorator
are different expressions of the same "varying behavior" theme.
