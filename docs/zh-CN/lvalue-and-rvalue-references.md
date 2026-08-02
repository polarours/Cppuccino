# Lvalue 与 Rvalue 引用

## C++ 引用系统的基础

在 C++ 中，引用是一个强大的概念，理解 lvalue 和 rvalue 引用是掌握现代 C++ 的关键。

## 基本概念

### Lvalue（左值）

Lvalue 是一个表示持久对象或函数的表达式。它可以在表达式之后仍然存在。

```cpp
int x = 42;           // x 是 lvalue
int& ref = x;         // lvalue 引用绑定到 lvalue
```

判断标准：**有名字、可以取地址**。

### Rvalue（右值）

Rvalue 是临时对象或即将销毁的对象。它没有持久身份。

```cpp
int y = x + 1;        // x + 1 的结果是 rvalue
int&& rref = 42;      // rvalue 引用绑定到 rvalue
```

判断标准：**无名字、临时、可以移动**。

## 引用类型

### 1. Lvalue 引用 (T&)

```cpp
int a = 10;
int& b = a;    // OK: lvalue 引用绑定到 lvalue
// int& c = 10;  // 错误: 非 const lvalue 引用不能绑定到 rvalue
```

### 2. 常量 Lvalue 引用 (const T&)

```cpp
const int& d = 10;  // OK: const lvalue 引用可以绑定到 rvalue
int e = 10;
const int& f = e;   // OK: const lvalue 引用绑定到 lvalue
```

### 3. Rvalue 引用 (T&&)

```cpp
int&& g = 10;       // OK: rvalue 引用绑定到 rvalue
// int&& h = e;     // 错误: rvalue 引用不能绑定到 lvalue
int&& i = std::move(e);  // OK: 显式转换为 rvalue
```

## 关键规则

### 规则 1：Lvalue 引用绑定规则

| 类型 | 可绑定的对象 |
|------|-------------|
| T& | 只绑 lvalue |
| const T& | 绑 lvalue 和 rvalue |
| T&& | 只绑 rvalue |

### 规则 2：引用折叠

在模板推导中，引用折叠规则适用：
- `T& &` → `T&`
- `T& &&` → `T&`
- `T&& &` → `T&`
- `T&& &&` → `T&&`

```cpp
template<typename T>
void perfectForward(T&& arg) {  // T&& 是转发引用
    // 如果传入 lvalue，T 推导为 T&，T&& 折叠为 T&
    // 如果传入 rvalue，T 推导为 T，T&& 保持 T&&
}
```

### 规则 3：临时对象生命周期延长

```cpp
const int& ref = 42;  // 临时对象生命周期延长到 ref 的作用域
```

只有 `const lvalue 引用` 才能延长临时对象的生命周期。

## 实际应用

### 1. 移动语义

```cpp
std::vector<int> v1 = {1, 2, 3};
std::vector<int> v2 = std::move(v1);  // 转移所有权，避免拷贝
```

### 2. 完美转发

```cpp
template<typename T>
void wrapper(T&& arg) {
    otherFunction(std::forward<T>(arg));  // 保持原始值的类型
}
```

### 3. 避免不必要的拷贝

```cpp
void process(const std::string& str);  // 只读访问，避免拷贝
void process(std::string str);         // 需要拥有，接受值
```

## 常见陷阱

### 陷阱 1：悬垂引用

```cpp
const int& ref = getTemp();  // 危险！临时对象已销毁
```

### 陷阱 2：误用 rvalue 引用

```cpp
void foo(int&& x) {
    // x 是 rvalue 引用，但在函数体内是 lvalue！
    int& ref = x;  // OK: x 现在是 lvalue
}
```

### 陷阱 3：引用绑定到删除的对象

```cpp
int* ptr = new int(42);
int& ref = *ptr;
delete ptr;
// ref 现在悬垂！
```

## 总结

- Lvalue 引用 (T&)：绑定 lvalue，表示"借用"
- 常量 Lvalue 引用 (const T&)：绑定 lvalue 和 rvalue，表示"只读访问"
- Rvalue 引用 (T&&)：绑定 rvalue，表示"可移动"
- 理解引用绑定规则是掌握移动语义和完美转发的基础
- 注意避免悬垂引用和误用 rvalue 引用

> **核心思想：引用是别名，不是对象。选择正确的引用类型可以明确表达代码的意图。**
