# C++20 Concepts 深入理解 (C++20 Concepts)

## 什么是 Concepts？

Concepts 是 C++20 引入的编译期概念系统，它让你可以**显式声明**模板参数的类型约束，而不是隐式依赖"这个类型恰好能用 `operator+`"这样的运行时行为。

> **核心思想**：把"类型要满足什么能力"从注释变成编译器能检查的契约。

## 基本概念语法

```cpp
#include <concepts>

// 自定义概念：可打印的类型
template<typename T>
concept Printable = requires(T a) {
    { std::cout << a } -> std::convertible_to<std::ostream&>;
};

// 自定义概念：可加的类型
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

// 组合概念
template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;
```

- `requires` 子句定义一组要求
- `{ expr } -> Concept` 表达返回值满足的概念
- `std::convertible_to<A, B>` / `std::same_as<A, B>` 是标准库预定义的概念

## 概念用于函数模板

```cpp
// 只有满足 Number 的类型才能调用
template<Number T>
T add(T a, T b) {
    return a + b;
}

// 不满足时编译报错，错误信息清晰
auto x = add(1, 2);       // OK
auto y = add("a", "b");   // 编译错误：string 不满足 Number
```

对比没有 concept 的情况：

```cpp
// 没有 constraint 时，错误在实例化时才出现，且信息晦涩
template<typename T>
T add(T a, T b) { return a + b; }
```

## 概念用于类模板

```cpp
#include <vector>
#include <ranges>

template<std::ranges::range R>
class ContainerWrapper {
public:
    explicit ContainerWrapper(R r) : data_(std::move(r)) {}
    
    auto begin() { return data_.begin(); }
    auto end()   { return data_.end(); }
    
private:
    R data_;
};
```

`std::ranges::range` 要求类型有 `begin()` 和 `end()`——编译器保证使用这个类模板的 R 一定是可遍历的。

## 约束别名（Constrained Alias）

```cpp
template<Number T>
using NumberRef = T&;

NumberRef<int> x = 42;  // 等价于 int&，但加了约束
```

## Compound Requirements 与 `->` 追踪返回类型

```cpp
template<typename T>
concept ContainerLike = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};
```

- `requires` 子句可以有"compound requirement"：`{ expr } -> Concept;`
- 不仅检查 `expr` 是否合法，还检查它的**返回值**是否满足 Concept

## `requires` 子句的两种形式

```cpp
// 形式一：requires clause（跟在模板参数之后）
template<Number T>
T add(T a, T b);

// 形式二：requires expression（嵌入概念定义）
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};
```

也可以用在成员函数上：

```cpp
class Calculator {
public:
    template<Number T>
    T calculate(T a, T b) requires(true) {
        return a + b;
    }
};
```

## 标准库概念速查

| 概念 | 要求 |
|---|---|
| `std::integral<T>` | 整数类型 |
| `std::floating_point<T>` | 浮点类型 |
| `std::same_as<A, B>` | A 与 B 同一类型 |
| `std::convertible_to<A, B>` | A 可隐式转换为 B |
| `std::copyable<T>` | 可拷贝构造、可拷贝赋值、可析构 |
| `std::movable<T>` | 可移动构造、可移动赋值、可析构 |
| `std::ranges::range<T>` | 有 `begin()`/`end()` |
| `std::random_access_range<T>` | 支持 `[]`、`<`、`-` |

## 概念的可满足性测试

```cpp
static_assert(std::integral<int>);
static_assert(!std::integral<double>);
static_assert(std::ranges::range<std::vector<int>>);
```

在编译期直接验证概念是否被正确定义。

## 最佳实践

1. **概念放在 header 里定义**：概念是模板声明的一部分，必须可见
2. **用具体概念命名**：`HasToString` 比 `Stringable` 更清晰；`Addable` 比 `Numeric` 更具体
3. **优先用 `requires` clause 而非 SFINAE**：编译错误信息更友好，诊断工具支持更好
4. **概念要"精确且简洁"**：`IntegralOrFloat` 用 `std::integral || std::floating_point` 表达，不要自定义冗长概念
5. **不要过度约束**：`requires std::size_t` 比 `requires T == std::size_t` 容易读

## 总结

Concepts 是 C++ 模板编程从"靠编译错误猜问题"走向"靠约束明确契约"的关键改进。它让 `template<typename T>` 变成 `template<Number T>`，编译错误从晦涩的类型推导失败变成"int 不满足 Number 概念"这样清晰的诊断。
