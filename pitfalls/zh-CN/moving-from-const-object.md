# 移动语义陷阱：从 const 对象移动

## 问题描述

在 C++ 中，尝试从 `const` 对象移动（move）是一个常见的陷阱。因为移动操作期望能够修改源对象（通常是将其置为空状态），而 `const` 对象不允许修改。

## 错误示例

```cpp
#include <iostream>
#include <string>
#include <utility>

int main() {
    const std::string str = "hello";
    
    // 错误！const 对象不能被移动
    std::string result = std::move(str);  // 编译错误！
    
    return 0;
}
```

编译错误：
```
error: cannot bind non-const lvalue reference of type 'std::string&' 
to an object of type 'const std::string'
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
3. 但移动操作需要非 const 的 rvalue 引用
4. 类型不匹配，编译失败

## 正确的做法

### 1. 不要对 const 对象移动

```cpp
std::string str = "hello";
std::string result = std::move(str);  // 正确：str 不是 const
```

### 2. 使用 const_reference 访问

```cpp
const std::string str = "hello";
const std::string& ref = str;  // 正确：只读访问
```

### 3. 需要拷贝时显式拷贝

```cpp
const std::string str = "hello";
std::string result = str;  // 正确：拷贝
```

### 4. 自定义 const 安全的移动

如果你需要在 `const` 上下文中"移动"（实际上是深度拷贝），可以定义额外的接口：

```cpp
class Data {
public:
    // 移动构造
    Data(Data&& other) noexcept : data_(std::exchange(other.data_, nullptr)) {}
    
    // 拷贝构造（用于 const 源）
    Data(const Data& other) : data_(other.data_ ? new int(*other.data_) : nullptr) {}
    
private:
    int* data_;
};
```

## 相关陷阱

1. **悬垂引用**：移动后源对象状态未定义
2. **从 const 移动**：移动操作要求非 const 源
3. **误以为 std::move 会移动**：它只是类型转换

## 总结

- `std::move` 只是类型转换，不保证实际移动
- 移动操作需要非 const 的 rvalue 引用
- 对 `const` 对象使用 `std::move` 会编译失败
- 如果需要对 `const` 对象操作，使用拷贝语义

> **核心原则：移动语义的核心是"偷"，而 `const` 对象不允许被"偷"。如果需要转移，请确保源对象不是 const。**
