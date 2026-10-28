# The Ranges Pattern In-Depth (Ranges Pattern)

## What Are Ranges?

C++20 Ranges is the declarative data-processing abstraction provided by the
`<ranges>` library. The core idea is the **data pipeline**: the `|` operator
chains filtering, mapping, and slicing steps into one flow — no manual loops
and no temporary containers.

> **Core idea**: lazy evaluation — each step produces only a view, and nothing
> is actually traversed until you consume it.

## Ranges vs Classic Algorithms

| | std::algorithm | std::ranges |
|---|---|---|
| Call style | `std::sort(v.begin(), v.end())` | `v \| std::views::sort` |
| Temporary containers | Build one before `copy_if` | views defer consumption; no extra allocation |
| Composability | Pass iterators again per algorithm | chained pipeline, more expressive |

## Range Adapters (View Adapters)

The heart of Ranges is its family of view adapters:

| View | Effect | Example |
|---|---|---|
| `filter(p)` | keep matching elements | `nums \| std::views::filter([](int n){ return n % 2 == 0; })` |
| `transform(fn)` | map each element | `names \| std::views::transform([](auto& s){ return s.size(); })` |
| `take(n)` | first N elements | `values \| std::views::take(5)` |
| `skip(n)` | drop first N | `items \| std::views::skip(2)` |
| `reverse()` | reversed order | `seq \| std::views::reverse` |
| `join()` | flatten nested sequences | `matrix \| std::views::join` |
| `keys()` / `values()` | unpack a sequence of pairs | `map \| std::views::keys` |

## Lazy Evaluation and Pipelines

```cpp
#include <ranges>
#include <vector>
#include <algorithm>
#include <iostream>

int main() {
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // pipeline: filter → transform → take
    auto result = nums
        | std::views::filter([](int n){ return n % 2 == 0; })  // [2,4,6,8,10]
        | std::views::transform([](int n){ return n * n; })    // [4,16,36,64,100]
        | std::views::take(3);                                  // [4,16,36]

    for (int n : result) std::cout << n << " ";  // 4 16 36
}
```

Note: `result` is not a `vector` — it is a view. The computation runs only
when the `for` loop iterates it.

## The Range Concept Hierarchy

| Concept | Requires | Meaning |
|---|---|---|
| `std::ranges::range` | `begin()`/`end()` | any iterable sequence |
| `std::ranges::input_range` | `range` + `input_iterator` | single pass |
| `std::ranges::forward_range` | `range` + `forward_iterator` | multi-pass |
| `std::ranges::bidirectional_range` | `forward_range` + bidirectional | iterate both ways |
| `std::ranges::random_access_range` | `bidirectional` + random access | supports `[]` |
| `std::ranges::sized_range` | `range` + `size()` | size known |
| `std::ranges::viewable_range` | wrappable by `std::views::all` | lifetime-safe range |

## Writing a Custom View

A custom view needs two pieces: an `iterator` (the cursor) and the view itself
(which owns the cursor's origin).

```cpp
#include <ranges>
#include <vector>
#include <cstddef>
#include <iterator>

class EvenView : public std::ranges::view_base {
    std::vector<int>& data_;
public:
    explicit EvenView(std::vector<int>& d) : data_(d) {}

    struct iterator {
        std::vector<int>* owner = nullptr;   // the sequence being iterated
        std::vector<int>::iterator cur;
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using iterator_category = std::input_iterator_tag;

        // advance to the next even element
        iterator& operator++() {
            const auto last = owner->end();
            while (++cur != last && *cur % 2 != 0) {}
            return *this;
        }
        int operator*() const { return *cur; }
        bool operator==(const iterator&) const = default;
    };

    iterator begin() {
        iterator it{&data_, data_.begin()};
        const auto last = data_.end();
        while (it.cur != last && *it.cur % 2 != 0) ++it.cur;  // skip leading odds
        return it;
    }
    iterator end() { return {&data_, data_.end()}; }
};
```

## Best Practices

1. **Prefer range-based calls**: `v | std::views::filter(...)` reads better
   than explicit iterator plumbing
2. **Mind view lifetimes**: a view does not own its data — iterating after the
   underlying container dies is UB
3. **Use `std::ranges::iota_view` instead of manual loops**:
   `for (int n : std::views::iota(1, 11))` yields 1..10
4. **`take_while` / `drop_while` beat manual `break`s** for readability
5. **`std::ranges::sort` requires a random_access_range**: fine for normal
   containers, but not for `std::list`

## Example

A runnable version of this topic:

- [std-ranges-advanced.cpp](../../examples/std-ranges-advanced.cpp)
- [range-based-for.cpp](../../examples/range-based-for.cpp)
- [views-chunk-split.cpp](../../examples/views-chunk-split.cpp)
- [cpp20-ranges.cpp](../../examples/cpp20-ranges.cpp)
- [cpp23-ranges-views.cpp](../../examples/cpp23-ranges-views.cpp)


## Summary

Ranges is C++'s step from "imperative iteration" to "declarative pipelines".
Together with the concept hierarchy, the compiler can produce far more
meaningful error messages. Understanding lazy view semantics and lifetimes is
the precondition for range code that is both fast and safe.
