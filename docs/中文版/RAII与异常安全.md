# RAII 与异常安全

## 1. 引言

RAII（Resource Acquisition Is Initialization）是 C++ 最核心的设计原则之一。
它不是“代码风格建议”，也不只是“避免泄漏”的技巧。

RAII 建立的是更强不变式：

> 资源生命周期绑定到对象生命周期，而对象生命周期受语言语义严格约束。

异常处理会带来非线性控制流。
若清理逻辑依赖手工分支，异常路径就会成为正确性薄弱点。

本章聚焦：

1. RAII 依赖的语言保证；
2. RAII 与异常安全级别的关系；
3. 生产级 C++ 代码中的可执行实践。

---

## 2. 讨论边界

### 2.1 本章覆盖

- 栈展开与确定性析构；
- 常见资源类别的 RAII 封装；
- 基本保证/强保证/不抛异常保证；
- 事务式提交（commit）模式；
- 常见反模式与评审清单。

### 2.2 本章不覆盖

不罗列所有标准库设施细节。
目标是建立工程推理框架，而非 API 背诵。

---

## 3. RAII 依赖的语言保证

RAII 可成立，前提是 C++ 对对象析构行为有确定性规定。

### 3.1 自动存储期对象

自动存储期对象在作用域退出时析构，
无论退出是正常路径还是异常路径。

### 3.2 逆序析构

同一作用域内，对象按构造逆序析构。
这天然形成“后构造先释放”的栈式资源管理。

### 3.3 栈展开期间的二次异常

若栈展开中析构函数再次抛出异常，程序调用 `std::terminate`。
这也是“析构函数应不抛异常”的根本原因。

---

## 4. RAII 类型的基本形态

一个稳健的 RAII 类型通常具备：

1. 构造时获取资源，失败即抛出；
2. 析构时释放资源且不抛异常；
3. 所有权语义显式（拷贝/移动策略清晰）。

示例：

```cpp
#include <cstdio>
#include <stdexcept>

class FileHandle {
public:
    explicit FileHandle(const char* path)
        : fp_(std::fopen(path, "w")) {
        if (!fp_) {
            throw std::runtime_error("fopen failed");
        }
    }

    ~FileHandle() noexcept {
        if (fp_) {
            std::fclose(fp_);
        }
    }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept : fp_(other.fp_) {
        other.fp_ = nullptr;
    }

    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this == &other) return *this;
        if (fp_) std::fclose(fp_);
        fp_ = other.fp_;
        other.fp_ = nullptr;
        return *this;
    }

    std::FILE* get() const noexcept { return fp_; }

private:
    std::FILE* fp_{};
};
```

---

## 5. 异常安全级别

### 5.1 基本保证（Basic Guarantee）

操作失败后：

1. 不泄漏资源；
2. 对象不变式仍成立。

### 5.2 强保证（Strong Guarantee）

操作具备事务语义：

- 要么完全成功；
- 要么外部可观察状态不变。

### 5.3 不抛异常保证（No-Throw Guarantee）

操作承诺不抛异常。
常用于析构、交换、底层基础设施接口。

RAII 提供结构基础，但最终保证级别仍由算法设计决定。

---

## 6. RAII 是结构能力，不是完整策略

RAII 负责“自动清理”。
它并不自动提供“回滚语义”。

若要强保证，通常仍需：

1. 构建影子状态；
2. 执行提交步骤；
3. 使用不抛异常交换/指针翻转。

---

## 7. 强保证的提交模式

```cpp
#include <vector>

class Buffer {
public:
    void replace_all(const std::vector<int>& src) {
        std::vector<int> tmp = src; // 可能抛
        data_.swap(tmp);             // 常见实现下 swap 为不抛
    }

private:
    std::vector<int> data_;
};
```

若 `tmp` 构造失败，`data_` 不变，从而实现强保证。

---

## 8. 析构函数与 `noexcept`

析构函数应默认不抛。
若释放过程可能失败，应通过显式接口汇报（如 `close()`、错误码、日志），
而不是在析构中传播异常。

```cpp
class Socket {
public:
    void close();          // 可报告错误
    ~Socket() noexcept;    // 最终清理，不抛
};
```

---

## 9. 所有权模型与 RAII

### 9.1 唯一所有权

典型工具：`std::unique_ptr`、文件句柄封装、锁守卫。

### 9.2 共享所有权

典型工具：`std::shared_ptr`。
仅在确有共享生命周期图时使用，避免“设计不清晰就上共享指针”。

### 9.3 借用关系

借用（引用/裸指针）不负责清理。
只有拥有者才负责释放资源。

---

## 10. 标准库中的 RAII 首选

1. `std::unique_ptr`：堆资源唯一拥有。
2. `std::shared_ptr`：共享拥有（需配合 `std::weak_ptr` 防循环）。
3. `std::lock_guard` / `std::scoped_lock`：锁生命周期管理。
4. `std::vector` / `std::string`：动态存储管理。
5. `std::jthread`（C++20）：线程生命周期管理。

能用标准 RAII 类型就不要重复造轮子。

---

## 11. 锁的 RAII 管理

```cpp
#include <mutex>

std::mutex m;
int shared_value = 0;

void update() {
    std::lock_guard<std::mutex> guard(m);
    ++shared_value; // 后续即使抛异常，也会自动解锁
}
```

若手工 `lock()` / `unlock()`，异常与早返回路径极易遗漏解锁。

---

## 12. 多资源获取

当函数需获取多个资源时，
推荐顺序构造多个 RAII 对象。

若第 N 个资源获取失败，已构造的前 N-1 个对象会自动析构并释放资源。
无需手写层层回收分支。

---

## 13. 部分构造与不变式

构造函数抛异常时，已完成构造的子对象会自动析构。

设计建议：

1. 成功构造后对象立即满足不变式；
2. 避免“半初始化可用”状态；
3. 资源成员尽量放在自管理类型中。

---

## 14. C API 封装中的 RAII

生产代码经常需调用 C 库。
这时 RAII 封装是防泄漏关键。

```cpp
#include <memory>

extern "C" {
    struct CHandle;
    CHandle* c_open();
    void c_close(CHandle*);
}

using CHandlePtr = std::unique_ptr<CHandle, void(*)(CHandle*)>;

CHandlePtr make_handle() {
    CHandle* raw = c_open();
    if (!raw) throw std::runtime_error("c_open failed");
    return CHandlePtr(raw, &c_close);
}
```

---

## 15. 常见反模式

1. 在 A 函数 `new`，在远处 B 函数 `delete`；
2. 手工加锁解锁包裹可抛代码；
3. 析构函数传播异常；
4. 用共享所有权掩盖对象边界不清；
5. 两阶段初始化（构造后再 `init()`）。

这些写法都会削弱异常安全。

---

## 16. 两阶段初始化问题

```cpp
class Bad {
public:
    Bad();
    void init(); // 可能失败
};
```

此模式会产生“对象存在但不可用”的灰区状态。
更稳健方案：

1. 构造成功即满足不变式；或
2. 工厂函数返回已完成初始化对象。

---

## 17. 工厂函数与错误建模

当构造流程复杂时，推荐工厂函数统一编排：

```cpp
class Service {
public:
    static Service create(/* params */);
};
```

工厂可组合多个 RAII 组件，并保证对外只暴露“完整对象”。

---

## 18. RAII 的性能误解

误解：RAII 增加对象数量，会更慢。

现实：

1. 大多数 RAII 包装器是零/低开销抽象；
2. 编译器常可消除样板层；
3. 与泄漏、死锁、异常路径缺陷相比，RAII 成本通常可忽略。

性能结论应以实测为准。

---

## 19. 异常中立（Exception-Neutral）

泛型代码应尽量异常中立：

- 不吞异常掩盖错误；
- 若下层抛异常，上层保持不变式；
- 与用户类型组合时语义稳定。

RAII 是异常中立设计的地基。

---

## 20. 移动后状态

RAII 类型移动后，被移动对象必须仍可析构且保持有效状态。
典型约定是“空拥有者”。

若移动后状态不清晰，会引入延迟缺陷。

---

## 21. 并发语境中的 RAII

RAII 不能替代并发协议设计，
但它能保证异常路径上的资源释放（如锁释放）具备确定性。

常见安全组合：

1. 进入作用域即构造锁守卫；
2. 修改共享状态；
3. 所有出口自动解锁。

---

## 22. 如何测试 RAII 合同

建议测试维度：

1. 构造失败路径；
2. 获取后、释放前插入异常；
3. Sanitizer 验证无泄漏；
4. 失败后对象不变式检查。

“没崩溃”不是异常安全证明。

---

## 23. 代码评审清单

合并前建议逐项确认：

- [ ] 所有权是否通过类型表达，而非注释假设？
- [ ] 析构函数是否有效 `noexcept`？
- [ ] 拷贝/移动策略是否清晰？
- [ ] 是否所有路径都自动清理？
- [ ] 各操作异常保证是否说明？
- [ ] C 资源是否在获取后立即封装？

若任一项模糊，应先重构再优化。

---

## 24. 工程准则汇总

1. 默认“一资源一拥有者”；
2. 优先标准 RAII 类型；
3. 析构不抛异常；
4. 强保证采用提交/回滚模式；
5. 避免两阶段初始化；
6. 将所有权与生命周期视为 API 契约。

---

## 25. 总结

RAII 把异常路径中的清理问题，从“分支补丁”升级为“类型契约”。

在稳健的 C++ 系统中：

1. 资源由对象拥有；
2. 所有权转移显式可审查；
3. 清理行为确定且自动；
4. 异常保证由设计提供，而非侥幸成立。

当这些条件成立时，异常安全将成为系统可组合性质，而不是事故后补救。

## 另请参阅

- **相关文档：** [所有权与生命周期](所有权与生命周期.md)，[未定义行为](未定义行为.md)，[noexcept与移动操作](noexcept与移动操作.md)
- **常见陷阱：** [析构函数抛出异常](../pitfalls/中文版/析构函数抛出异常.md)，[未初始化成员变量](../pitfalls/中文版/未初始化成员变量.md)
- **示例：** [raii-and-exception-safety.cpp](../examples/raii-and-exception-safety.cpp)
