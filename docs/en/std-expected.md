# std::expected — 错误处理的类型安全解 (C++23)

## 什么是 std::expected？

`std::expected<T, E>` 是 C++23 引入的类型，它表示一个值**或者**一个错误——成功时包含 `T`，失败时包含 `E`。用类型系统把"有值"和"有错"这两种互斥状态显式编码，编译期强制调用方处理错误。

> **核心思想**：`std::optional` 的安全版——不是"可能没有"，而是"有值或有错"。

## 与 std::optional 的对比

| | std::optional | std::expected |
|---|---|---|
| 语义 | "可能没有" | "有值或有错" |
| 失败时携带信息 | 无 | 携带错误类型 `E` |
| monadic API | 有 | 有（`and_then`/`transform`/`or_else`） |
| 适用场景 | 可选参数、找不到时返回空 | 所有需要报错路径的函数 |

## 核心概念

- **成功路径**：`return expected;` 或 `return value;`（隐式从 `T` 构造）
- **失败路径**：`return std::unexpected(error);`
- **解包**：`if (r)` 判断，`*r` 取值，`.error()` 取错，`.value_or(default)` 降级

## 代码实现

```cpp
#include <expected>
#include <string>

enum class ErrorCode {
    None, NotFound, InvalidInput, NetworkError
};

std::string errorMessage(ErrorCode code) {
    switch (code) {
        case ErrorCode::NotFound:    return "Not found";
        case ErrorCode::InvalidInput: return "Invalid input";
        case ErrorCode::NetworkError: return "Network error";
        default: return "No error";
    }
}

// 返回值或错误
std::expected<int, ErrorCode> parseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::unexpected(ErrorCode::InvalidInput);
    }
}

std::expected<double, ErrorCode> divide(double a, double b) {
    if (b == 0) return std::unexpected(ErrorCode::InvalidInput);
    return a / b;
}
```

完整可运行示例见 [examples/std-expected.cpp](../../examples/std-expected.cpp)。

## Monadic 操作（链式调用）

`std::expected` 支持 `and_then` / `transform` / `or_else`，让错误在链式调用中自然传播：

```cpp
// parseInt("42") → *2 → /2 → 结果
auto result = parseInt("10")
    .and_then([](int v) { return divide(v, 2.0); });  // 5.0

// transform：不改变错误类型，只改变值类型
auto doubled = parseInt("21")
    .transform([](int v) { return v * 2; });  // 42

// or_else：自定义错误处理
auto safe = parseInt("abc")
    .or_else([](ErrorCode) { return std::unexpected(ErrorCode::NetworkError); });
```

## 最佳实践

1. **优先 `std::expected` 而非 `std::pair<bool, T>`**：前者有命名接口，类型系统强制处理，错误信息不泄漏
2. **优先 `std::unexpected` 而非 `throw`**：C++ 哲学是"不抛异常的函数不抛"——`expected` 把这个契约显式化
3. **错误类型用 `enum class`**：避免与成功类型混淆；必要时用 `std::error_code` 或自定义 struct
4. **不要在 monadic 链中混合类型**：`.and_then` 的闭包必须也返回 `std::expected`，否则编译报错
5. **用 `.value_or()` 降级时要注意**：它只在 expected 为空时执行默认值表达式——如果默认值本身计算量很大，考虑用 `or_else`

## 与 std::error_code 的关系

大型系统推荐把错误类型设为 `std::error_code`，它本身可组合、可比较、已存在于标准库生态中：

```cpp
std::expected<int, std::error_code> readConfig(const std::string& key);
```

## 总结

`std::expected` 是 C++ 从"错误靠异常或返回值约定"走向"类型系统强制错误处理"的关键一步。配合 monadic API 可以写出错误安全（error-safe）的链式调用，逻辑清晰且不会遗漏错误分支。
