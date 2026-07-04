# `noexcept` 与移动操作

## 1. 引言

`noexcept` 在 C++ 中不是“注释性标记”，而是可执行契约。
它直接影响：

1. 泛型代码是否能采用更激进但安全的路径；
2. 标准容器在元素迁移时选择移动还是拷贝；
3. 异常安全保证能否成立。

在移动语义场景中，这一影响尤为明显。
如果类型的移动构造可能抛异常，容器常会选择拷贝路径以维持强异常安全。

本章核心目标是：

> 明确何时应为移动操作声明 `noexcept`，以及如何在不“虚假承诺”的前提下设计该契约。

---

## 2. 讨论边界

### 2.1 本章覆盖

- `noexcept` 的语言语义；
- 移动构造/移动赋值与 `noexcept` 的关系；
- 容器策略选择背后的异常安全考量；
- 条件 `noexcept` 的实用模式；
- 工程评审清单。

### 2.2 本章不覆盖

不展开所有 STL 实现细节差异。
我们关注可移植、可证明的语义层结论。

---

## 3. `noexcept` 的语义本质

### 3.1 契约含义

函数被声明为 `noexcept`，表示异常不得向外传播。
若异常仍越过边界，程序将调用 `std::terminate`。

因此，`noexcept` 是行为约束，不是建议性说明。

### 3.2 泛型中的可查询属性

`noexcept` 可参与编译期分派：

```cpp
static_assert(noexcept(std::declval<int&>() = 1));
```

标准库与泛型组件可基于 `std::is_nothrow_move_constructible_v<T>` 等 trait 选择策略。

---

## 4. 容器为何关心“移动是否不抛”

以 `std::vector` 扩容为例，常见流程是：

1. 分配新内存；
2. 迁移旧元素；
3. 释放旧内存。

为了维持强异常安全，迁移阶段若存在抛异常风险，容器可能改用拷贝策略。

这意味着：

- nothrow move 往往解锁更理想迁移路径；
- throwing move 可能触发保守回退。

---

## 5. 最小示例与影响

```cpp
class Node {
public:
    Node() = default;
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;

    Node(const Node&) = default;
    Node& operator=(const Node&) = default;
};
```

该声明向泛型调用方明确传达：`Node` 的移动是非抛异常的。
容器在重定位元素时更可能选择移动路径。

---

## 6. 何时不能无条件写 `noexcept`

若成员移动本身可能抛异常，而你仍无条件声明 `noexcept`，
就会把运行期失败升级为 `terminate` 风险。

```cpp
class Risky {
public:
    Risky(Risky&&) noexcept {
        // 若内部子对象移动可抛，则契约有风险
    }
};
```

工程准则：

> 只有在可证明“整条移动路径均不抛”时，才声明无条件 `noexcept`。

---

## 7. 条件 `noexcept`：组合类型首选

```cpp
template <class Member>
class Wrapper {
public:
    Wrapper() = default;

    Wrapper(Wrapper&&)
        noexcept(std::is_nothrow_move_constructible_v<Member>) = default;

    Wrapper& operator=(Wrapper&&)
        noexcept(std::is_nothrow_move_assignable_v<Member>) = default;

private:
    Member m_;
};
```

该写法可让契约随成员特性自动收敛，降低“类型演进后契约失真”的风险。

---

## 8. 移动构造与移动赋值不是同一问题

两者风险面不同：

1. 移动构造：从源对象构建新对象；
2. 移动赋值：对既有对象进行状态替换，常伴随旧资源释放。

因此，一个类型可能“移动构造不抛”但“移动赋值可能抛”。
不要机械对称声明。

---

## 9. 零法则（Rule of Zero）与 `noexcept`

若类型直接由标准拥有型成员构成（如 `std::unique_ptr`、`std::vector`），
采用 Rule of Zero 通常更稳健。

手写特殊成员函数应基于真实语义需求，而非习惯。

---

## 10. `std::move_if_noexcept` 的意义

`std::move_if_noexcept(x)` 的核心策略：

- 若移动构造可证明不抛（或无拷贝），返回 `T&&`；
- 否则返回 `const T&`，偏向拷贝路径。

```cpp
template <class T>
void relocate(T& x) {
    T y(std::move_if_noexcept(x));
}
```

你的类型 `noexcept` 设计会直接影响这类分派行为。

---

## 11. 强异常安全视角

强保证通常意味着：操作要么成功完成，要么外部可观察状态不变。

若移动可抛，泛型组件维持强保证的代价会显著上升，
甚至需要回退到更保守策略。

因此，nothrow move 常是“保证结构”的关键条件，而非单纯性能优化。

---

## 12. RAII 与移动后状态

即便移动不抛，也必须保证：

1. 移动后对象处于有效可析构状态；
2. 资源所有权只转移一次；
3. 异常路径无泄漏。

`noexcept` 不能替代正确所有权建模。

---

## 13. 类型分类（实务视角）

### 13.1 常见可不抛移动类型

- 句柄窃取型轻量封装；
- `std::unique_ptr` 风格所有权对象；
- 仅由不抛移动成员构成的聚合。

### 13.2 常见可能抛移动类型

- 移动过程仍需分配资源；
- 复杂不变式维护中存在可失败步骤；
- 依赖第三方组件且其移动语义不稳定。

先分类，再声明契约。

---

## 14. 高频反模式

1. 为“性能看起来更好”而盲目加 `noexcept`；
2. 模板包装器漏掉条件 `noexcept`；
3. 成员变化后忘记同步审查 `noexcept` 契约；
4. 把“几乎不抛”当成“绝不抛”。

这些问题会在泛型代码中放大。

---

## 15. 类型演进风险

项目迭代中，类型新增成员后移动语义可能变化。
若旧的无条件 `noexcept` 未更新，契约会悄然失效。

建议：

1. 使用默认特殊成员 + 条件 `noexcept`；
2. 在关键类型上加入 trait 静态断言；
3. 将 noexcept 特性变化视为接口级变化。

---

## 16. 测试与验证建议

建议采用分层验证：

1. 编译期 trait 检查；
2. 运行期 moved-from 状态一致性测试；
3. Sanitizer 检测资源与生命周期问题；
4. 容器扩容场景压力测试。

目标是验证“契约一致性”，而不仅是“功能可用”。

---

## 17. 诊断工具片段

```cpp
template <class T>
constexpr void require_nothrow_move() {
    static_assert(std::is_nothrow_move_constructible_v<T>,
                  "T must be nothrow-move-constructible");
}
```

在策略依赖 nothrow move 的泛型组件中，可用该断言防止契约退化。

---

## 18. API 设计建议

公开类型应明确：

1. 拷贝/移动的异常保证；
2. moved-from 状态约定；
3. 是否承诺稳定 `noexcept` 属性；
4. 若属性变化，是否属于破坏性升级。

调用方会基于这些属性构建性能与正确性预期。

---

## 19. 代码评审清单

合并前建议逐项核对：

- [ ] 移动构造 `noexcept` 是否有证明。
- [ ] 移动赋值 `noexcept` 是否单独论证。
- [ ] 组合类型是否使用条件 `noexcept`。
- [ ] 是否存在“声明不抛但内部可抛”的路径。
- [ ] trait 变化是否被文档化。
- [ ] 对容器行为影响是否被评估。

若任一项不明确，应收紧声明并补充说明。

---

## 20. 版本备注

- C++11 引入移动语义与 `noexcept`；
- 现代标准库广泛基于 nothrow trait 做策略分派。

具体实现细节可因库版本不同而异，但“契约驱动策略”这一方向稳定可靠。

---

## 21. 对比示例

```cpp
struct A {
    A(A&&) noexcept = default;
    A(const A&) = default;
};

struct B {
    B(B&&) = default; // 可能抛
    B(const B&) = default;
};
```

在泛型迁移场景下，`A` 更可能走移动优先路径，
`B` 则可能触发拷贝回退策略。

语义差异不在“语法形态”，而在异常契约。

---

## 22. 总结

`noexcept` 是移动语义中的关键接口契约，直接影响泛型策略、容器行为和异常安全边界。

可靠的工程实践应遵循：

1. 可证明才声明；
2. 组合类型优先条件 `noexcept`；
3. 将 trait 变化视作接口变化；
4. 正确性优先于“看起来更快”的短期优化。

当契约与实现一致时，`noexcept` 才能真正成为性能与可靠性的共同增益点。
