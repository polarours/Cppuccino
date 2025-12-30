# const 并不是“不变”，而是一种接口契约

在 C++ 中，`const` 常被误解为“对象不可修改”。
这种理解并不错误，但非常不完整。更准确地说，`const` 是一种接口层面的承诺（interface contract）：

> 一句话总结就是：我向调用者保证：通过这个接口，你所能观察到的对象状态不会发生变化。

## 什么是 const？

### 逻辑状态 vs 物理状态

在讨论 `const` 之前，必须区分两个层次的“状态”：

- **逻辑状态（logical state）**
  - 指对象对外表现出来的行为和属性。例如，一个计数器对象的逻辑状态是它当前的计数值。
- **物理状态（physical state）**
  - 指对象内部的实际数据存储和实现细节。例如，计数器对象可能会有一些缓存变量或辅助数据结构，这些是它的物理状态的一部分。

`const` 约束的是逻辑状态，而不是物理内存是否发生写入。

我们考虑以下例子：

```cpp
class Counter {
public:
    void increment() {
        ++count_;
        cache_valid_ = false;
    }

    int getCount() const {
        if (!cache_valid_) {
            cached_value_ = count_; // 修改物理状态（仅用于缓存），逻辑状态 count_ 不变
            cache_valid_ = true;
        }
        return cached_value_;
    }

private:
    int count_ = 0;

    // 以下成员不属于逻辑状态
    // mutable 并不是破坏 const，而是用于显式标注：该成员不属于对象的逻辑状态。
    mutable bool cache_valid_ = false;
    mutable int cached_value_ = 0;
};

int main() {
    Counter c;
    c.increment();
    int value = c.getCount(); // 通过 const 方法访问
    return 0;
}
```

在这个例子中，`getCount` 方法被声明为 `const`，这意味着它不会修改 `Counter` 对象的逻辑状态（即 `count_`）。然而，它确实修改了 `cache_valid_` 和 `cached_value_` 这两个成员变量，这些变量属于物理状态。

> 注意：逻辑状态和物理状态的划分并非语言强制，而是设计理念。实际项目中，合理划分有助于性能优化和接口设计。

### const 的真正含义

当我们使用 `const` 修饰一个对象时，我们是在告诉编译器和读者：“通过这个接口，我承诺不会修改这个对象的逻辑状态。” 这并不意味着对象的物理状态完全不可变。 例如，某些成员变量可能被标记为 `mutable`，允许在 `const` 方法中修改它们。

我们看下面这个例子：

```cpp
class Example {
public:
    int getValue() const {
        accessCount++; // 允许修改，因为 accessCount 是 mutable
        return value;
    }
private:
    int value = 42;
    mutable int accessCount = 0; // 这个成员变量可以在 const 方法中修改
};
```

在这个例子中，`accessCount` 被声明为 `mutable`，因此即使在 `getValue` 方法中（该方法是 `const`），我们仍然可以修改 `accessCount`。这表明 `const` 并不意味着对象的所有成员都不可变，而是强调了逻辑状态的不变性。

## 深入剖析 const 的设计哲学

### 接口契约

在 C++ 中，“接口契约”（Interface Contract）是一种设计理念，它强调通过函数签名、类型修饰符、类接口等手段向使用者明确规定对象或函数的行为规范。简单说，就是 “我承诺通过这个接口做什么、不会做什么”。
在 `const` 的语境下，这种契约意味着：通过一个 `const` 接口访问对象时，调用者可以信赖该接口不会改变对象的语义状态（逻辑状态）。

> 当然接口契约不仅仅局限于 `const`，还包括其他方面，比如异常安全保证、线程安全保证等。

### 性能优化

使用 `const` 还可以帮助编译器进行优化。因为编译器知道某些对象不会被修改，它可以更大胆地进行优化，比如缓存结果、消除不必要的拷贝等。

比如我们可以写一个简单的测试程序来对比 const 和非 const 方法的性能差异，具体的演示代码请参考[const_vs_nonconst.cpp](/examples/const_vs_nonconst.cpp)。

### 代码可读性和维护性

使用 `const` 可以提高代码的可读性和维护性。通过明确标记哪些对象是不可修改的，开发者可以更容易地理解代码的意图，减少误用和错误。

例如：

```cpp
void printValue(const int& value) {
    std::cout << value << std::endl;
}
```

在这个例子中，`const int& value` 明确表示 `printValue` 函数不会修改传入的参数，这有助于调用者理解函数的行为。

## 总结

- `const` 并不是“对象不可变”，而是一种接口契约，承诺不会修改对象的逻辑状态。
- 区分逻辑状态和物理状态有助于理解 `const` 的真正含义。
- 使用 `const` 可以提高代码的性能、可读性和维护性。

