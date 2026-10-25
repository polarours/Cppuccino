# 移动语义陷阱：从 const 对象移动

## 问题描述

在 C++ 中，尝试从 `const` 对象移动（move）是一个常见的陷阱。因为移动操作期望能够修改源对象（通常是将其置为空状态），而 `const` 对象不允许修改。

## 示例：静默拷贝，而非移动

```cpp
#include <iostream>
#include <string>
#include <utility>

int main() {
    const std::string str = "hello";

    // 陷阱：编译通过，但发生的是拷贝，不是移动
    std::string result = std::move(str);

    return result == "hello" ? 0 : 1;
}
```

**不报错才是最危险的**：重载解析发现移动构造 `string(string&&)` 接受不了
`const string&&`，于是回落到拷贝构造 `string(const string&)`。你以为转移了
资源，其实每个字节都复制了一遍。

真正会编译报错的两种情形：

```cpp
#include <memory>
#include <string>
#include <utility>

void demo() {
    // 情形 A：只移动类型（拷贝被删除）→ 编译错误
    const std::unique_ptr<int> p = std::make_unique<int>(42);
    auto q = std::move(p);

    // 情形 B：结果绑定到非 const 引用 → 编译错误
    const std::string str = "hello";
    std::string& ref = std::move(str);
}
```

## 为什么会有这个问题？

移动构造函数和移动赋值运算符通常接受非 const 的 rvalue 引用：

```cpp
class MyClass {
public:
    MyClass(MyClass&& other) noexcept;           // 移动构造
    MyClass& operator=(MyClass&& other) noexcept; // 移动赋值
};
```

当你对 `const` 对象使用 `std::move` 时：
1. `std::move` 只是进行类型转换，不实际移动任何东西
2. 转换结果是一个 `const T&&` 类型的 rvalue 引用
3. 移动操作需要非 const 的 `T&&`，`const T&&` 绑不上
4. 于是重载解析回落到拷贝构造 `const T&`（可拷贝类型）→ **静默拷贝**；
   若拷贝已被删除（只移动类型）→ 编译错误

## 正确的做法

### 1. 不要对 const 对象移动

```cpp
#include <string>
#include <utility>

std::string str = "hello";
std::string result = std::move(str);  // 正确：str 不是 const
```

### 2. 使用 const_reference 访问

```cpp
#include <string>

const std::string str = "hello";
const std::string& ref = str;  // 正确：只读访问
```

### 3. 需要拷贝时显式拷贝

```cpp
#include <string>

const std::string str = "hello";
std::string result = str;  // 正确：拷贝
```

### 4. 自定义 const 安全的移动

如果你需要在 `const` 上下文中"移动"（实际上是深度拷贝），可以定义额外的接口：

```cpp
#include <utility>

class Data {
public:
    // 移动构造
    Data(Data&& other) noexcept : data_(std::exchange(other.data_, nullptr)) {}

    // 拷贝构造（用于 const 源）
    Data(const Data& other) : data_(other.data_ ? new int(*other.data_) : nullptr) {}

    ~Data() { delete data_; }

private:
    int* data_;
};
```

## 相关陷阱

1. **悬垂引用**：移动后源对象状态未定义
2. **从 const "移动"**：可拷贝类型会静默拷贝，只移动类型会编译失败
3. **误以为 std::move 会移动**：它只是类型转换

## 示例

这个主题的可运行版本：

- [pitfall-const-object-move-trap.cpp](../../examples/pitfall-const-object-move-trap.cpp)

## 总结

- `std::move` 只是类型转换，不保证实际移动
- 移动操作需要非 const 的 rvalue 引用
- 对 `const` 对象使用 `std::move` 不会移动：可拷贝类型静默拷贝，
  只移动类型编译失败
- 如果需要对 `const` 对象操作，使用拷贝语义

> **核心原则：移动语义的核心是"偷"，而 `const` 对象不允许被"偷"。如果需要转移，请确保源对象不是 const。**
