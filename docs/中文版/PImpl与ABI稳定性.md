# PImpl 与 ABI 稳定性

## 1. 引言

C++ 的 ABI（Application Binary Interface）对类型布局与符号细节高度敏感。
当头文件暴露私有成员时，内部实现的局部改动也可能引发：

1. 下游大面积重编译；
2. 二进制兼容破坏；
3. 发布与升级风险增加。

PImpl（Pointer to Implementation，也称 Cheshire Cat 惯用法）本质是边界控制手段：

> 将稳定接口与可变实现分离，以降低 ABI 波动和编译耦合。

本章不仅讨论“如何写”，还讨论“何时值得写”。

---

## 2. 讨论边界

### 2.1 本章覆盖

1. ABI 易碎性的来源；
2. PImpl 的标准结构与特殊成员函数设计；
3. 运行时与构建时成本；
4. 实际工程中的采用决策。

### 2.2 本章不覆盖

不按平台逐条讲解 ABI 细则。
我们关注跨平台普适的工程结论。

---

## 3. 为什么 C++ ABI 容易破裂

### 3.1 私有字段也会影响二进制形态

即使私有成员不在 API 注释中出现，它仍会影响：

- 对象大小与对齐，
- 内联函数生成代码，
- 符号依赖结构，
- 异常展开相关信息。

因此，“源代码接口未变”并不等于“ABI 未变”。

### 3.2 头文件暴露会放大依赖图

私有实现类型被放进头文件后，会沿 include 链扩散。
后果包括：

1. 编译时间膨胀；
2. 模块边界变弱；
3. 修改成本与风险上升。

---

## 4. PImpl 的核心思想

公开类只保留稳定接口与一个指向实现体的指针。
具体实现（字段、私有算法、依赖头）全部移动到 `.cpp`。

这样可将“经常变化的内容”隔离在二进制边界内部。

---

## 5. 基础骨架

```cpp
// widget.h
#pragma once
#include <memory>

class Widget {
public:
    Widget();
    ~Widget();

    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;

    Widget(const Widget&);
    Widget& operator=(const Widget&);

    void draw();

private:
    struct Impl;
    std::unique_ptr<Impl> p_;
};
```

`Impl` 在 `.cpp` 中定义：

```cpp
// widget.cpp
#include "widget.h"

struct Widget::Impl {
    // 私有字段与实现细节
};
```

---

## 6. 析构函数为何常放到 `.cpp`

`std::unique_ptr<Impl>` 在销毁时需要完整类型。
若析构在头文件内联生成，调用点可能缺失 `Impl` 完整定义。

常见稳健做法：

1. 头文件中仅声明 `~Widget();`
2. `.cpp` 中（`Impl` 定义之后）写 `Widget::~Widget() = default;`

这样既保持封装，又避免不完整类型相关问题。

---

## 7. 移动语义与 PImpl

对 `std::unique_ptr` 成员来说，移动通常只涉及所有权转移。

```cpp
Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;
```

这种写法通常正确且高效，moved-from 对象保持有效但内容可为空。

---

## 8. 拷贝语义需要显式设计

`unique_ptr` 不可拷贝，因此“值语义 Widget”必须定义深拷贝策略。

```cpp
Widget::Widget(const Widget& other)
    : p_(other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr) {}

Widget& Widget::operator=(const Widget& other) {
    if (this == &other) return *this;
    p_ = other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr;
    return *this;
}
```

拷贝策略至少要在文档中明确：

1. 深拷贝，或
2. 共享状态，或
3. 明确禁拷贝。

---

## 9. 异常安全建议

拷贝赋值涉及内存分配时，推荐先分配后交换，以保持强保证。

```cpp
Widget& Widget::operator=(const Widget& other) {
    if (this == &other) return *this;
    auto tmp = other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr;
    p_.swap(tmp);
    return *this;
}
```

若分配失败，原对象状态保持不变。

---

## 10. const 语义仍需严格兑现

即便实现被隐藏，公开接口的 `const` 承诺依然是契约。
若 `Impl` 内部使用缓存或惰性计算，应明确：

1. 逻辑 const 是否成立；
2. 并发读场景是否线程安全。

---

## 11. ABI 稳定收益

PImpl 在以下方面明显受益：

1. 公开类型布局稳定；
2. 内部字段变更不直接扩散到客户端；
3. 私有依赖不再污染公共头。

对于长期维护库，这些收益通常非常现实。

---

## 12. 构建收益

私有实现依赖转移到 `.cpp` 后，头文件更轻，依赖图更窄。
大型工程常见效果包括：

- 重编译范围缩小；
- 头文件解析压力下降；
- 模块边界更清晰。

---

## 13. 运行时成本

PImpl 的典型成本：

1. 一次指针间接访问；
2. `Impl` 动态分配；
3. 可能的缓存局部性损失；
4. 样板代码增多。

这些成本在粗粒度 API 中常可接受，
但在极端热路径应实测评估。

---

## 14. 性能优化变体（谨慎使用）

可选优化包括：

1. 自定义分配器管理 `Impl`；
2. arena/pool 分配；
3. 小对象内联缓冲（SBO 化 PImpl）。

它们会显著提高复杂度，仅在数据证明必要时采用。

---

## 15. 内联机会与 LTO

实现隐藏到 `.cpp` 可能减少跨 TU 内联机会（具体取决于工具链和 LTO）。
若接口调用极短且极高频，应量化评估该影响。

PImpl 与 LTO 并不冲突，但不能假设 LTO 总能完全抵消边界成本。

---

## 16. 与 C++ Modules 的关系

Modules 可显著改善编译模型，但并不自动解决二进制分发中的 ABI 演进问题。
PImpl 仍是“接口稳定、实现可变”的直接手段。

两者可协同使用：

- Modules 解决构建组织，
- PImpl 解决二进制边界稳定。

---

## 17. 符号可见性与版本策略

PImpl 只是 ABI 策略的一部分。
完整方案还应包括：

1. 符号可见性控制（如默认隐藏）；
2. 导出 API 白名单；
3. 版本化与弃用流程。

否则即便使用 PImpl，也可能因其他符号暴露导致兼容风险。

---

## 18. 常见错误

1. 析构函数在头文件内联，导致不完整类型问题；
2. 没有定义清晰的拷贝策略；
3. 误以为 PImpl 自动提供线程安全；
4. 对小型值对象滥用 PImpl；
5. 通过 inline 辅助函数泄露 `Impl` 细节。

---

## 19. 适用场景

以下条件越多，PImpl 越值得：

1. 需要长期 ABI 兼容承诺；
2. 内部实现会频繁演进；
3. 项目对重编译成本敏感；
4. 指针间接成本可接受。

---

## 20. 不适用场景

以下情况应谨慎：

1. 紧密热路径的微小值类型；
2. 性能强依赖紧凑布局与内联；
3. 项目始终源码重编译且不关注 ABI 分发；
4. 类型生命周期极短且频繁构造销毁。

---

## 21. 完整示例（含五大特殊成员）

```cpp
// widget.h
#pragma once
#include <memory>

class Widget {
public:
    Widget();
    ~Widget();

    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;

    Widget(const Widget&);
    Widget& operator=(const Widget&);

    void draw() const;

private:
    struct Impl;
    std::unique_ptr<Impl> p_;
};
```

```cpp
// widget.cpp
#include "widget.h"
#include <utility>

struct Widget::Impl {
    int width = 0;
    int height = 0;
    void draw() const {
        // render details
    }
};

Widget::Widget() : p_(std::make_unique<Impl>()) {}
Widget::~Widget() = default;

Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;

Widget::Widget(const Widget& other)
    : p_(other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr) {}

Widget& Widget::operator=(const Widget& other) {
    if (this == &other) return *this;
    auto tmp = other.p_ ? std::make_unique<Impl>(*other.p_) : nullptr;
    p_.swap(tmp);
    return *this;
}

void Widget::draw() const {
    p_->draw();
}
```

该模式在保持公开布局稳定的同时，仍可提供可用值语义。

---

## 22. 评审清单

引入或修改 PImpl 前建议核对：

- [ ] ABI 稳定是否确为项目目标；
- [ ] 析构函数定义位置是否正确；
- [ ] 拷贝/移动策略是否明确且有测试；
- [ ] 异常安全级别是否可说明；
- [ ] 公共头是否仍泄露私有依赖；
- [ ] 运行时成本是否经过测量。

---

## 23. 旧类型迁移建议

迁移步骤可采用渐进式：

1. 固定公开 API 形态；
2. 引入前置声明 `Impl` 与指针成员；
3. 私有字段迁移至 `.cpp`；
4. 重建并验证特殊成员语义；
5. 执行 ABI 与性能回归检查。

有测试护栏时，迁移可控且风险可管理。

---

## 24. 总结

PImpl 是工程层面的战略取舍，而非“默认最佳实践”。

它带来：

1. 更稳固的 ABI 边界；
2. 更低的编译耦合；
3. 更清晰的封装层次。

它也引入：

1. 一次间接访问与分配成本；
2. 更高样板代码复杂度。

当项目确有长期兼容与边界治理需求时，PImpl 往往是高性价比方案；
若缺乏该前提，则应谨慎评估其额外成本。
