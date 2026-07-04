# C++ 内存模型与原子操作基础

## 1. 引言

并发代码的正确性不能由“在本机跑了几次没问题”来证明。
C++ 内存模型定义了跨线程可见性的合法边界，以及哪些行为直接落入未定义行为（UB）。

其中最核心的规则是：

> 数据竞争（Data Race）是未定义行为。

本章聚焦工程落地所需的基础能力：

1. 如何用 happens-before 推理可见性；
2. 各内存序到底承诺了什么；
3. 哪些常见并发模式是“语言层面可证明正确”的。

---

## 2. 讨论边界

### 2.1 本章覆盖

- 数据竞争定义与后果；
- 原子类型与原子操作；
- `release/acquire` 发布-订阅模式；
- RMW（读改写）与 CAS 循环；
- 工程实践与评审清单。

### 2.2 本章不覆盖

不展开完整 lock-free 数据结构实现。
本章目标是建立可靠的并发语义基础。

---

## 3. 必须精确使用的术语

### 3.1 线程内顺序

单线程内有“sequenced-before”关系，描述表达式/语句执行先后。
但该顺序不会自动跨线程传播。

### 3.2 数据竞争

若满足以下条件即构成数据竞争：

1. 两个线程访问同一内存位置；
2. 访问冲突（至少一个是写）；
3. 缺乏同步关系将其有序化。

一旦构成数据竞争，程序行为未定义。

### 3.3 happens-before

若 `A happens-before B`，则 `A` 的效果对 `B` 可见。
这是并发正确性证明的核心关系。

### 3.4 modification order

每个原子对象上的修改形成该对象自己的全序。
注意这是“每个对象各自的全序”，不是所有原子共享一个全局全序。

---

## 4. 原子类型保证了什么

`std::atomic<T>` 为可支持类型提供原子操作。

“原子”仅表示对该对象的操作不可分割。
它不自动为其他普通变量提供可见性顺序。

```cpp
std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);
```

上例保证 `counter` 自身增量原子化，
但不保证与其他内存访问形成同步关系。

---

## 5. 内存序总览

### 5.1 `memory_order_relaxed`

- 仅保证原子性；
- 不建立同步边；
- 常用于统计计数等“顺序不敏感”场景。

### 5.2 `memory_order_release`

- 常用于写入（store 或带 release 语义的 RMW）；
- 保证同线程中 release 之前的写入，对匹配 acquire 可见。

### 5.3 `memory_order_acquire`

- 常用于读取（load 或带 acquire 语义的 RMW）；
- 禁止后续读写重排到 acquire 之前；
- 可与同一原子对象上的 release 建立同步。

### 5.4 `memory_order_acq_rel`

- 用于 RMW 场景；
- 同时具备 acquire 与 release 语义。

### 5.5 `memory_order_seq_cst`

- 最强、最易推理；
- 对顺序一致原子建立全局单序；
- 在部分架构上可能代价更高。

---

## 6. 发布-订阅：正确基线模式

```cpp
#include <atomic>

std::atomic<bool> ready{false};
int payload = 0;

void producer() {
    payload = 42;
    ready.store(true, std::memory_order_release);
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)) {
    }
    // 此处读取 payload 可见 42
}
```

推理链条：

1. `payload = 42` 在 producer 中先于 release-store；
2. consumer 的 acquire-load 读取到该 release 写入；
3. 建立 synchronizes-with；
4. 进而形成 happens-before，可见性成立。

---

## 7. 错误示例：用 relaxed 做发布信号

```cpp
void producer_bad() {
    payload = 42;
    ready.store(true, std::memory_order_relaxed);
}

void consumer_bad() {
    while (!ready.load(std::memory_order_relaxed)) {
    }
    // 对 payload 可见性无语言保证
}
```

即便在某些机器上“看起来正常”，也不构成标准语义保证。

---

## 8. 读改写（RMW）与 CAS

RMW 操作包括：

- `fetch_add` / `fetch_sub`，
- `exchange`，
- `compare_exchange_weak/strong`。

它们对同一原子对象保持原子性竞争行为。

```cpp
std::atomic<int> x{0};
x.fetch_add(1, std::memory_order_acq_rel);
```

当操作既要消费先前同步，又要发布后续状态时，`acq_rel` 常是合理选择。

---

## 9. CAS 循环样式

```cpp
std::atomic<int> value{0};

void increment_if_even() {
    int expected = value.load(std::memory_order_relaxed);
    while ((expected % 2) == 0) {
        if (value.compare_exchange_weak(
                expected,
                expected + 1,
                std::memory_order_acq_rel,
                std::memory_order_relaxed)) {
            break;
        }
        // 失败时 expected 会被更新
    }
}
```

要点：

1. `weak` 允许伪失败，循环重试是预期路径；
2. 失败内存序不能强于成功内存序；
3. 代码必须在竞争下保持收敛与正确性。

---

## 10. Fence：可用但不宜滥用

`atomic_thread_fence` 可建立顺序约束，但可读性与可证明性通常更差。

工程建议：

1. 优先使用与操作绑定的内存序（release/acquire）；
2. 仅在必要时使用 fence，并补充分层注释与证明。

---

## 11. `volatile` 不是线程同步工具

在 C++ 中，`volatile` 主要用于特殊内存访问语义（如 MMIO）。
它不提供线程间同步保证。

不要用 `volatile` 代替 `std::atomic` 或互斥锁。

---

## 12. 原子性不等于不变式一致性

原子操作保护的是“单个内存位置的操作原子化”。
若你的不变式涉及多个字段协同更新，单个原子不足以覆盖。

应使用：

1. 互斥锁保护复合状态；或
2. 经严格证明的 lock-free 协议。

---

## 13. 架构经验不能替代语言证明

“x86 很强，所以这样写没问题”不是 C++ 语义证明。

跨平台与长期维护中，问题常在以下条件变化时出现：

- CPU 架构更换；
- 编译器版本升级；
- 优化级别变更。

正确性必须建立在语言模型上。

---

## 14. 团队内存序选择策略

实用流程：

1. 先用 `seq_cst` 建立正确性基线；
2. 通过测试与评审固定协议不变式；
3. 再有目标地放松到 acquire/release 或 relaxed；
4. 最后在真实负载上评估收益。

过早“降序”是并发缺陷高发源。

---

## 15. 验证与测试现实

并发 bug 高度依赖时序，偶发且难复现。
“没测出错”不等于“没有错误”。

建议组合验证：

1. 压力测试与随机调度扰动；
2. ThreadSanitizer 竞态检测；
3. 代码评审中的 happens-before 证明；
4. 协议状态断言。

---

## 16. relaxed 的合理使用边界

`relaxed` 适用于：

1. 仅需原子性，
2. 不承担发布/同步信号角色，
3. 允许读取到滞后值。

典型例子是遥测计数器：

```cpp
std::atomic<unsigned long long> events{0};

void record() {
    events.fetch_add(1, std::memory_order_relaxed);
}
```

---

## 17. 指针发布模式示例

```cpp
struct Config {
    int threshold;
};

std::atomic<Config*> g_cfg{nullptr};

void publish(Config* p) {
    g_cfg.store(p, std::memory_order_release);
}

Config* acquire_cfg() {
    return g_cfg.load(std::memory_order_acquire);
}
```

若 acquire 读到非空指针，则可见 release 之前对 `Config` 的初始化写入。

---

## 18. 何时应优先 `std::mutex`

以下场景优先互斥锁：

1. 复合状态一致性要求高；
2. 原子协议复杂到难以局部证明；
3. 可维护性优先于理论极限性能；
4. 实际争用水平并不支持 lock-free 的复杂度投入。

“无锁”并不天然更快，也不天然更可靠。

---

## 19. 代码评审清单

合并前至少确认：

1. 是否存在清晰的 happens-before 证明链；
2. 是否完全消除数据竞争；
3. 内存序是否“最小但充分”；
4. `relaxed` 是否仅用于顺序不敏感场景；
5. 是否避免了架构特定假设；
6. 是否评估了互斥锁替代方案。

若证明无法清晰表达，优先简化方案。

---

## 20. 文档术语规范

建议统一使用：

- “未定义行为（UB）”描述数据竞争后果；
- “synchronizes-with”描述 release/acquire 同步边；
- “happens-before”描述可见性推理关系。

避免“通常可见”“大概率正确”这类模糊措辞。

---

## 21. 版本备注

- C++11 引入正式内存模型与原子库；
- 后续标准持续扩展原子生态；
- 本章核心正确性原则在现代标准中保持稳定。

示例和结论应明确目标标准模式。

---

## 22. 总结

正确的并发 C++ 必须从语言语义出发，而非 CPU 经验。

推荐工程路径：

1. 先定义并发不变式；
2. 再建立 happens-before 证明；
3. 选择满足证明的内存序；
4. 用工具与压力测试验证；
5. 最后再做性能优化。

当团队遵循这一顺序，原子操作就是可推理的精密工具，而不是不可控风险源。
