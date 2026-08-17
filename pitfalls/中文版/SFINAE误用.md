# SFINAE 误用

## 引言

SFINAE——*替换失败并非错误（Substitution Failure Is Not An Error）*——指的是：在为某个重载推导签名时发生无效的类型替换，只会**把该重载从候选集中移除**，而不是让整个编译失败。它是「仅当类型支持 X 时才启用这个重载」的经典机制。

坑在于：SFINAE **只对模板声明即时上下文（immediate context）内的替换生效**——即其参数列表、返回类型或模板实参。如果你的约束写在函数**体**里，或任何即时上下文之外的地方，一次失败的替换就会变成**硬编译错误**，而不是安静地移除。

## 问题

```cpp
#include <vector>

// 错误：约束是在函数体内部检查的。
// 若 T 没有 .size()，这是一个硬错误，而非 SFINAE。
template <typename T>
void call_size(const T& t) {
    t.size();           // 当 T 缺少 .size() 时编译错误
    // （甚至在重载决议能「丢弃」这个模板之前，它就先报错了）
}
```

第二类常见误用：把约束写得太晚，于是模板先被选中，再在深处失败，产生一个远离真正原因的、令人费解的错误。

**为什么出错：**
- SFINAE 只能挽救「推导函数签名时」发生的替换。像函数体里的 `t.size()` 这样的调用是普通代码；若 `T` 没有 `size()`，编译器报的是普通的「没有名为 'size' 的成员」错误——它无法「反悔」不选这个重载。
- 结果是冗长、令人困惑的错误信息，而本该兜底的那个重载根本没机会被选中。

## 修复

把约束移到签名里，让失败成为「替换失败（软）」而不是「函数体错误（硬）」。一个小 trait 配合 `std::enable_if` 是经典写法：

```cpp
#include <iostream>
#include <type_traits>
#include <vector>

template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

// 约束位于签名中 -> 对 SFINAE 安全
template <typename T>
std::enable_if_t<has_size<T>::value, std::size_t>
call_size(const T& t) {
    return t.size();    // 此刻可保证 T 拥有 .size()
}

int main() {
    std::vector<int> v{1, 2, 3};
    return call_size(v) == 3 ? 0 : 1;   // 通过 SFINAE 选中该重载
    // call_size(42);  // 只是「不匹配」 -> 不会产生硬错误
}
```

在 C++17 及之后，`if constexpr` 或 concepts（`requires` / `std::enable_if` 的替代）通常更清晰、诊断更好——但核心规则一致：**在签名里检查，而不是在函数体里。**

## 最佳实践

- **把约束写进签名**（返回类型、模板参数，或者——最好——C++20 的 `requires` 子句）。绝不要依靠函数体去「拒绝」某个类型。
- **优先用 concepts（C++20）而非手写的 SFINAE**——当可用时：`template <typename T> requires requires(T t){ t.size(); }` 可读性高得多，错误信息也更好。
- **用 `std::void_t` traits** 探测成员/类型；这是标准的 SFINAE 惯用法，且组合性好。
- **盯紧即时上下文边界。** 如果你的「约束」能移到返回类型的 `decltype(...)` 里或默认模板实参里，就这么做——那正是它从「硬」变「软」的关键。
- **不要过度约束。** 每个 `enable_if` 都在收窄重载集；两个约束重叠的重载会变为歧义，所以让各条件互斥。

## 总结

SFINAE 是一个安全阀，而非调试工具：它把一次无效的替换变成安静的「这个重载不是候选」。破坏它的误用，是在函数体里检查约束——那里的失败是硬错误。把约束留在签名里（traits + `enable_if`，或 C++20 的 `requires`），SFINAE 才能尽职；把它们写进函数体，你得到的就是一个费解编译失败。
