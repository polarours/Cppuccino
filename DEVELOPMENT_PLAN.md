# Cppuccino 文档开发计划

## 当前状态

| 类型 | 英文 | 中文 | 总计 |
|------|------|------|------|
| 学习文档 (docs) | 67 篇 | 67 篇 | 134 篇 |
| 陷阱文章 (pitfalls) | 37 篇 | 37 篇 | 74 篇 |
| 示例代码 (examples) | - | - | 120 个 |
| 项目练习 | - | - | 151 个 |

## 第一阶段：补充核心概念文档 (预计 2-3 天)

### 1.1 设计模式文档 (Stage 8-27)

| 优先级 | 文档主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | 工厂方法模式 (Factory Method) | 中英 | 已完成 |
| P0 | 单例模式 (Singleton) | 中英 | 已完成 |
| P0 | 观察者模式 (Observer) | 中英 | 已完成 |
| P0 | 策略模式 (Strategy) | 中英 | 已完成 |
| P1 | 命令模式 (Command) | 中英 | 已完成 |
| P1 | 适配器模式 (Adapter) | 中英 | 已完成 |
| P1 | 装饰器模式 (Decorator) | 中英 | 已完成 |
| P1 | 建造者模式 (Builder) | 中英 | 已完成 |
| P2 | 原型模式 (Prototype) | 中英 | 已完成 |
| P2 | 代理模式 (Proxy) | 中英 | 已完成 |
| P2 | 组合模式 (Composite) | 中英 | 已完成 |
| P2 | 外观模式 (Facade) | 中英 | 已完成 |
| P3 | 其余 GoF 模式 | 中英 | 已完成 |

### 1.2 架构模式文档 (Stage 28-31)

| 优先级 | 文档主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | MVC 模式 | 中英 | 已完成 |
| P0 | 仓储模式 (Repository) | 中英 | 已完成 |
| P0 | CQRS 模式 | 中英 | 已完成 |
| P1 | 事件溯源 (Event Sourcing) | 中英 | 已完成 |
| P1 | 单元工作模式 (Unit of Work) | 中英 | 已完成 |
| P1 | 活动记录模式 (Active Record) | 中英 | 已完成 |
| P2 | 其余架构模式 | 中英 | 已完成 |

### 1.3 现代 C++ 文档 (Stage 35-38, 40-43)

| 优先级 | 文档主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | 内存池与分配器 | 中英 | 已完成 |
| P0 | 现代 C++ 错误处理 (std::expected) | 中英 | 已完成 |
| P0 | C++ 测试模式 | 中英 | 已完成 |
| P1 | C++20/23 新特性详解 | 中英 | 已完成 |
| P1 | CMake 与构建系统 | 中英 | 已完成 |
| P2 | SIMD 与性能优化 | 中英 | 已完成 |
| P2 | 协程深入理解 | 中英 | 已完成 |
| P2 | Ranges 深入理解 | 中英 | 已完成 |
| P3 | Concepts 深入理解 | 中英 | 已完成 |

## 第二阶段：完善陷阱文章 (预计 1-2 天)

### 2.1 并发相关陷阱

| 优先级 | 文档主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | 数据竞态 (Data Race) | 中英 | 已完成 |
| P0 | 死锁 (Deadlock) | 中英 | 已完成 |
| P1 | 优先级翻转 (Priority Inversion) | 中英 | 已完成 |
| P1 | 活锁 (Livelock) | 中英 | 已完成 |
| P2 | 线程泄漏 | 中英 | 已完成 |

### 2.2 内存相关陷阱

| 优先级 | 文档主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | 内存泄漏 (Memory Leak) | 中英 | 已完成 |
| P0 | 双重释放 (Double Free) | 中英 | 已完成 |
| P1 | 野指针 (Wild Pointer) | 中英 | 已完成 |
| P1 | 栈溢出 (Stack Overflow) | 中英 | 已完成 |
| P2 | 内存碎片 | 中英 | 已完成 |

### 2.3 泛型与模板陷阱

| 优先级 | 文档主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | 模板特化陷阱 | 中英 | 已完成 |
| P0 | SFINAE 误用 | 中英 | 已完成 |
| P1 | 类型推导失败 | 中英 | 已完成 |
| P1 | 模板实例化顺序 | 中英 | 已完成 |
| P2 | 折叠表达式错误 | 中英 | 已完成 |

## 第三阶段：补充示例代码 (预计 2-3 天)

### 3.1 设计模式示例

| 优先级 | 示例主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | 工厂模式示例 | C++20 | 已完成 |
| P0 | 单例模式示例 (含线程安全) | C++20 | 已完成 |
| P0 | 观察者模式示例 | C++20 | 已完成 |
| P0 | 策略模式示例 | C++20 | 已完成 |
| P1 | 命令模式示例 | C++20 | 已完成 |
| P1 | 适配器模式示例 | C++20 | 已完成 |
| P1 | 装饰器模式示例 | C++20 | 已完成 |
| P2 | 其余 GoF 模式示例 | C++20 | 已完成 |

### 3.2 架构模式示例

| 优先级 | 示例主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | MVC 示例 | C++20 | 已完成 |
| P0 | 仓储模式示例 | C++20 | 已完成 |
| P1 | CQRS 示例 | C++20 | 已完成 |
| P1 | 事件溯源示例 | C++20 | 已完成 |

### 3.3 现代 C++ 示例

| 优先级 | 示例主题 | 语言 | 状态 |
|--------|----------|------|------|
| P0 | std::expected 完整示例 | C++23 | 已完成 |
| P0 | std::span 完整示例 | C++20 | 已完成 |
| P1 | std::ranges 完整示例 | C++20 | 已完成 |
| P1 | std::coroutine 示例 | C++20 | 已完成 |
| P2 | std::format 高级用法 | C++20 | 已完成 |
| P2 | std::jthread 示例 | C++20 | 已完成 |
| P3 | std::source_location 示例 | C++20 | 已完成 |

## 第四阶段：完善项目测试与文档 (预计 2-3 天)

### 4.1 完善 Stage 4-7 项目文档

| 优先级 | 项目 | 任务 | 状态 |
|--------|------|------|------|
| P0 | simple-db | 完善 README，添加 API 文档 | 已完成 |
| P0 | chat-server | 完善 README，添加测试用例 | 已完成 |
| P1 | web-server | 添加性能基准测试 | 已完成 |
| P1 | task-scheduler | 完善错误处理文档 | 已完成 |
| P2 | connection-pool | 添加并发安全分析 | 已完成：计数/容量/工厂异常回归及并发示例已运行 |
| P2 | ring-buffer | 完善生产者-消费者文档 | 已完成 |

### 4.2 统一项目结构

- 所有项目添加 CMake 构建系统
- 所有项目添加单元测试
- 所有项目添加中英文 README
- 所有项目添加性能基准测试（可选）

## 第五阶段：更新项目元数据 (预计 0.5 天)

### 5.1 更新 README

- 更新文档统计数字
- 添加新的学习路径建议
- 完善贡献指南

### 5.2 更新 ROADMAP

- 检查 Stage 1-43 的链接有效性
- 添加缺失的文档引用
- 更新示例代码链接

## 开发原则

1. **质量优先**：每个文档都必须经过编译验证、逻辑检查
2. **中英对照**：每个新文档都需要同步中文版
3. **示例先行**：每个概念文档都配有可运行的示例代码
4. **陷阱对应**：每个核心概念都有对应的陷阱文章
5. **测试覆盖**：所有代码示例都有测试验证

## 进度追踪

| 阶段 | 预计完成时间 | 实际完成时间 | 状态 |
|------|-------------|-------------|------|
| 第一阶段 | - | 已完成 | ✅ 全部 P0/P1/P2 项已完成 |
| 第二阶段 | - | 已完成 | ✅ 全部已完成 |
| 第三阶段 | - | 已完成 | ✅ 全部已完成 |
| 第四阶段 | - | 已完成 | ✅ 全部已完成（web-server / task-scheduler / connection-pool 已运行验证） |
| 第五阶段 | - | 已完成 | ✅ 统计已更新，82 处坏链已修复 |

## 目标统计

完成所有开发后，项目将达到：

| 类型 | 英文 | 中文 | 总计 | 当前 |
|------|------|------|------|------|
| 学习文档 | 40 篇 | 40 篇 | 80 篇 | 134 篇 |
| 陷阱文章 | 35 篇 | 35 篇 | 70 篇 | 74 篇 |
| 示例代码 | - | - | 120 个 | 120 个 |
| 项目练习 | - | - | 151 个 | 151 个 |

## 第六阶段：docs/en 英文正文欠账（2026-10-04 发现）

> **2026-10-21 复核**：下列 15 篇未勾选文档经脚本复核确认正文**仍为中文**
> （此前一轮统计因把代码块内的拉丁字符计入正文，误判为已完成）。当前
> `docs/en/` 67 篇中，52 篇正文已是英文，15 篇待译，即下方清单。
>
> **2026-10-21 完成**：15 篇已全部译为英文（38/38）。`docs/en/` 现有 67 篇
> 全部为英文正文，`docs/中文版/` 67 篇全部为中文，标题骨架 15/15 一一对应。
> 翻译过程中修掉了 12 处真实代码缺陷（见第八阶段），中英两版均已修正且
> 可编译运行。

**问题**：`docs/en/` 62 篇中 38 篇（34 篇纯中文 + 4 篇中英混合）正文不是英文，
其中 32 篇与 `docs/中文版/` 对应文件**逐字节相同**（中文副本，从未翻译），合计
11,810 行。文件名是英文 kebab-case，但打开全是中文——按文件名统计的"英文 67 篇"
掩盖了这一欠账。

**完成判据**（每篇）：
1. 正文 CJK 字符数 < 拉丁字母数（脚本判定 EN-body；代码注释也应为英文）
2. 所有 ```cpp 块 g++ 编译通过（预期失败块须给出正确错误原因）
3. 与中文版章节骨架一一对应，代码块结构一致
4. 修完一批跑一次全仓链接校验（0 坏链）

**清单（进度 38/38，按行数从小到大；每批完成后勾掉）**：

- [x] service-locator-pattern.md (103L)
- [x] delegation-pattern.md (106L)
- [x] active-record-pattern.md (112L)
- [x] singleton-pattern.md (112L)
- [x] unit-of-work-pattern.md (119L)
- [x] strategy-pattern.md (119L)
- [x] observer-pattern.md (120L)
- [x] type-object-pattern.md (121L)
- [x] factory-method-pattern.md (124L)
- [x] mvc-pattern.md (158L)
- [x] repository-pattern.md (144L)
- [x] balking-pattern.md (133L)
- [x] simd-and-performance-optimization.md (133L)
- [x] std-expected.md (96L, MIXED)
- [x] ranges-pattern.md (111L, MIXED)
- [x] cmake-and-build-systems.md (153L, MIXED)
- [x] cpp20-concepts.md (161L, MIXED)
- [x] memory-pool-and-allocators.md (170L)
- [x] testing-patterns.md (282L)
- [x] adapter-pattern.md (421L)
- [x] chain-of-responsibility-pattern.md (446L)
- [x] bridge-pattern.md (462L)
- [x] flyweight-pattern.md (464L)
- [x] decorator-pattern.md (472L)
- [x] template-method-pattern.md (472L)
- [x] state-pattern.md (475L)
- [x] facade-pattern.md (480L)
- [x] prototype-pattern.md (483L)
- [x] visitor-pattern.md (484L)
- [x] builder-pattern.md (485L)
- [x] iterator-pattern.md (487L)
- [x] abstract-factory-pattern.md (491L)
- [x] composite-pattern.md (494L)
- [x] proxy-pattern.md (501L)
- [x] specification-pattern.md (504L)
- [x] memento-pattern.md (511L)
- [x] mediator-pattern.md (520L)
- [x] command-pattern.md (581L)

**规则**：翻译时保留章节骨架与代码块结构（代码注释译为英文）；不删除、不移动
中文版；每完成一篇把清单勾掉（进度以第六阶段勾选数为准；"当前状态"表按文件数
统计，不因翻译改变）。

## 第七阶段：仓库结构与统计口径修正（2026-10-21）

**问题**：全仓体检发现四处偏差——

1. `docs/` 根目录散落 5 篇文档（`chat-server-api.md`、`simple-db-api.md`、
   `task-scheduler-api.md`、`glossary.md`、`connection-pool-api.md`），
   全部是孤儿（无任何文件链接到它们）。其中 3 篇正文仍是中文，与
   `docs/中文版/` 对应文件逐字节相同（副本）。
2. README 声称"134 篇（67 en + 67 zh）"，但 `docs/en/` 实为 62 篇——差的
   5 篇正是上述根目录文档。
3. README / README_zh-CN 声称测试数 848，实际 `void test_*` 函数为 853 个
   （该数字自 commit `a6d8f01` 起就与实际不符）。
4. `docs/en/` 与 `docs/中文版/` 有两对文档标题骨架不同步：
   `copy-elision-and-prvalue-materialization`（英 40 / 中 38）、
   `virtual-dispatch-and-object-model`（英 30 / 中 29）。

**处理**：

- 5 篇根目录文档全部迁入 `docs/en/`（其中 3 篇译为英文），并按新深度修正
  相对链接（`../` → `../../`）；`docs/中文版/` 的对应文件保持不动。
- 修正 `copy-elision`（补 8.1 / 8.2 小节）与 `virtual-dispatch`
  （补「另请参阅」）的中文版骨架。
- 测试数 848 → 855（含本次新增的 2 个 chat-server 回归测试）。
- 修正 `docs/en/glossary.md`：原文件把 RAII / Rule of * / Range 等 R 开头的
  词条误置于 `## Q` 段，缺少 `## R` 段；已拆分并补入 Regular Expression。

**附带修复**：`chat-server` 的 `stop()` 会永久挂起——`close()` 不会唤醒阻塞在
`accept()` / `recv()` 的线程，导致 `join()` 无限等待。改用 `shutdown()` 唤醒，
并给 `send()` 加上 `MSG_NOSIGNAL`（否则对端已关闭会触发 SIGPIPE 杀掉进程）。
已补 2 个回归测试验证：修复前测试挂起（超时），修复后通过。

**当前真实口径**（按文件数统计）：

| 类型 | 英文 | 中文 | 总计 | 备注 |
|------|------|------|------|------|
| 学习文档 | 67 | 67 | 134 | 英文正文 67 篇全部就绪（第六阶段 38/38） |
| 陷阱文章 | 37 | 37 | 74 | 正文均为英文/中文 |
| 示例代码 | - | - | 120 | 全部可编译 |
| 项目练习 | - | - | 151 | 全部构建通过、ctest 全绿 |
| 测试函数 | - | - | 876 | 全部通过 |


## 第八阶段：第六阶段收尾——15 篇设计模式文档译为英文（2026-10-21）

**范围**：第六阶段剩余的 15 篇（flyweight、template-method、state、facade、
prototype、visitor、builder、iterator、abstract-factory、composite、proxy、
specification、memento、mediator、command），中英两版同步修订。

**做法**：按行数分 5 批并行处理。每篇要求：

1. 正文与代码注释全部译为英文，ASCII 图标签一并翻译；
2. 标题层级、数量、顺序与中文版 1:1 对应；
3. 代码块结构与行数保持不变（除修复缺陷所必需）；
4. 「完整示例」（含 `int main()`）必须能编译**并实际运行**；
   示意性片段（引用未定义 `T`、用 `...` 占位等）保持原样，与既有基线一致。

**顺带修掉的 13 处真实代码缺陷**（中英两版均已修正）：

| 文档 | 缺陷 |
|------|------|
| builder | `construct(const PizzaBuilder&)` 调用非 const 成员，整个示例无法编译 |
| iterator | `virtual currentItem() const = 0;` 缺返回类型；`SafeIterator` 缺 `list_` 成员；用 `make_unique<Iterator>` 实例化抽象类且与既有变量重名 |
| abstract-factory | 4 个工厂用 `override` 却没有继承基类 |
| flyweight | `std::unordered_map` 无 `std::pair` 哈希（C++20）；漏 `#include <vector>` |
| state | 字符串被中英混杂污染；注释与代码行为矛盾 |
| memento | `undo()` 返回 `unique_ptr` 而非引用，示例无法编译 |
| mediator | `User::send()` 查自己那张从未填充的表，聊天示例**静默不投递任何消息** |
| command | `DECLARE_COMMAND` 宏用 `action##_off` 拼出 `on_off`，展开即非法；`undo()` 语义反了 |
| specification | 组合规格缺前置声明；`shared_from_this()` 在依赖基类中需 `this->`；黑名单规格与数值规格类型不匹配却参与 `and_` 组合 |
| prototype | `Document` 无 `getCell()`，克隆后调用的是基类接口 |
| visitor | `Circle` / `Rectangle` / `Triangle` 缺前置声明，示例无法编译 |
| composite | `ComponentBase` 缺公共基类，`Composite` 无法同时持有叶节点和组合节点；`Node` / `SafeNode` 缺 `enable_shared_from_this` 却调用 `shared_from_this()` |

**验证**：中英两版各 15 篇，13 篇含可运行示例的**全部编译并运行通过**；
15/15 标题骨架一一对应；全仓相对链接 0 坏链；150 个项目 ctest 全绿。

## 第九阶段：把文档校验固化为工具（2026-10-21）

**动机**：前两轮的问题都源于"没有自动检查"。15 篇文件以英文文件名、中文正文
躺在 `docs/en/` 里好几个月，全靠人工翻阅才被发现；两次统计口径出错，也是因为
判定规则只存在于脑子里。

**产出**：

1. `cpp-mastery-roadmap/39-stage39/bilingual-doc-checker/` —— C++ 库
   （纯函数，不接触文件系统），提供语言判定、标题/代码块提取、代码等价比较、
   中英配对比较。17 个测试用例。
2. `tools/check_docs.py` —— 语言检查（覆盖全部 134 篇）+ 尽力配对后的
   标题/代码一致性检查。
3. `tools/check_doc_examples.py` —— 拼装每篇文档的完整示例，编译并运行。

**工具本身修掉的缺陷**（都是写工具时暴露的）：

- 并发运行共用 `/tmp` 固定文件名导致互相覆盖 → 改为按进程隔离的临时目录
- 示例拼装过于贪心，会把两段独立示例拼到一起 → 改为从最近的块向前回溯，
  取第一个能编译成功的最小集合
- 只用 `-std=c++20` → 改为依次回退 c++20 / c++23 / c++26
- 硬编码端口导致示例因端口占用而失败 → 改为传入一个空闲端口

**校验工具当场查出的真实问题**（已修）：

- `docs/中文版/任务调度器API文档.md` 用了 `std::this_thread` 却没 `#include <thread>`
- `docs/中文版/左值引用和右值引用.md` 缺「另请参阅」段，标题数与英文版差 1

**已知未覆盖**：7 篇文档因中英两半的代码差异过大，无法用结构指纹配对，
只报告"无法配对"而不做判定（`ownership-starts-here` 的英文版明显是旧版，
`reference-binding-rules` 中英代码块数 13 vs 29）。这类需要人工逐篇对齐。

## 第十阶段：配对检查全量收敛（2026-10-21）

**背景**：第九阶段引入的配对检查一开始只能验证 60/67 对中英文文档，
剩下 7 篇报告"无法配对"。逐篇排查后确认，其中多数并非工具问题，
而是真实的内容漂移。

**工具本身的改进**：

- 代码比较从"压缩空白后比字符串"改为**按词法单元比较**。原先会把
  `template<typename T>` 与 `template <typename T>` 判为不同——两者语义相同，
  只是模板参数列表前的空格位置不同。这一改进直接消除了 `cpp20-concepts`
  等文档的误报。
- 配对增加回退：无代码块的文档（术语表、构建系统说明）改用标题骨架配对。

**逐篇查出的真实漂移（已修）**：

| 文档 | 问题 |
|------|------|
| `ownership-starts-here` | 英文版是旧稿（11 个标题 / 1 个代码块），中文版已重构（16 个标题 / 7 个代码块）。英文版按中文版重译 |
| `raii-and-exception-safety` | 英文版多一个 `<utility>` 包含；锁守卫变量名 `g` 与 `guard` 不一致。已对齐 |
| `decorator-pattern` | 中文版 `TransparentDecorator`、`StatefulDecorator` 缺 `using CoffeeDecorator::CoffeeDecorator;`，照抄无法构造；英文版 `TemplatedDecorator::describe()` 漏了 `override`；中文版多层装饰缺 `demoLayers()` 函数与输出 |
| `术语表` | 中文版有一个空的 `## Q` 段——里面的 RAII、零/三/五法则全部以 R 开头。已并入 `## R`，并补上「范围适配器」条目；「未定义行为」原本在 B 段和 U 段各出现一次（U 段还是指向自己的"见未定义行为"），已按英文版改为 B 段 `未定义的行为（Behavior, Undefined）` + U 段交叉引用；中文版还缺 31 个英文版已有的词条（位域、缓冲区溢出、Concept、consteval、constinit、constexpr、条件变量、堆、同构容器、整型提升、内联函数、汇合、锁、字面量、多线程、成员变量、窄化转换、运算符重载、指针、谓词、span、结构化绑定、语句、存储类、线程安全、类型推导、类型擦除、元组等），已补齐并逐段对齐 |

**结果**：`docs/en` 67 篇与 `docs/中文版` 67 篇**全部 67 对配对成功**，
语言检查、标题骨架、代码块数量与内容全部一致，0 问题。

## 第十一阶段：把检查挂进 CI（2026-10-21）

**动机**：第九、十阶段的检查只在本地手动跑过一次。文档类的失效（英文名
中文正文、中英两半漂移、链接失效、示例编不过）恰恰是最容易引入、又最
不容易被肉眼发现的 —— 没人会逐篇翻 134 篇文档，但 CI 可以每次都翻。

**产出**：

| 文件 | 作用 |
|------|------|
| `.github/workflows/docs.yml` | 文档检查，每次 push 都跑 |
| `.github/workflows/build.yml` | 全部项目构建+测试，按 10 个分片并行 |
| `tools/run_all_checks.sh` | 本地与 CI 的统一入口 |
| `tools/check_links.py` | 相对链接与锚点校验 |
| `tools/build_all_projects.py` | 逐个配置、构建、测试全部项目 |
| `tools/plan_ci_shards.py` | 按项目数把阶段均衡分到分片 |

**设计要点**：CI 跑的就是本地那几个脚本（`tools/run_all_checks.sh`），
所以本地跑绿了 CI 就一定跑绿，不存在"CI 才发现的问题"。分片计划由脚本
算出，新增项目不需要改 workflow。

**立刻查出的真问题**：

1. `check_links.py` 增加了锚点校验后，查出 README 与 README_zh-CN 里指向
   `00-overview.md` 的 GoF 23 种模式链接**锚点写错了** —— 实际标题是
   `### Stage 8 Projects (08-stage8)`，锚点应为 `#stage-8-projects-08-stage8`，
   而链接写成了 `#stage-8-projects-8-stage8`（少了一个 0）。此前的链接检查
   只验文件是否存在，不验锚点，所以一直没发现。
2. 补上 `examples/` 的编译检查后，查出 **17 个示例的头注释编译命令是坏的**：
   文件名早已从下划线改成 kebab-case，头注释却还写着旧名字
   （`g++ ... copy_elision.cpp`，而文件叫 `copy-elision.cpp`），照抄必然报
   "no such file"；另有 `template-instantiation.cpp` 声明 `-std=c++17`，
   但里面用了 `requires`（C++20 特性）。均已修正，86 条单行编译命令现已
   逐条验证可执行。
3. `tinyshell` 子模块此前**从未在任何 CI 里被构建过**，现已加入。

**补上的 CI 覆盖漏洞**：加 CI 时发现两个洞 —— `examples/` 下 120 个独立示例
一次都没被编译过，`tinyshell` 子模块也没被构建过。现已分别由
`tools/check_examples.py` 和 build workflow 的 `tinyshell` job 覆盖。

## 第十二阶段：陷阱文章对齐与仓库卫生（2026-10-21）

**背景**：前几阶段只覆盖了 `docs/`，`pitfalls/` 这 37 篇陷阱文章从未做过双语
一致性检查 —— 和当初 `docs/en/` 里的 15 篇是同一类风险。

**做法**：陷阱文章的中文文件名是翻译（`悬垂指针` 对 `dangling-pointer`），
没有可匹配的拼写；结构指纹在两篇文章用了相似示例时又会配错。因此改为
**显式配对表** `tools/pitfall_pairs.tsv`，37 对逐条人工确认。

**查出的 11 处真实漂移**（全部已修）：

| 文档 | 问题 |
|------|------|
| `dangling-pointer` | 中文版把 `createInt` 定义了两次（重定义，编不过）；对 `int` 调用 `doSomething()`；用 `fopen` 却没 include `<cstdio>` |
| `object-slicing` | 5 个代码块各自漂移，其中一整段示例被挪了位置 |
| `throwing-from-destructor` | 中文版缺一节（英文版有"显式调用析构函数"），导致后面编号整体错位 |
| `iterator-invalidation` | 中文版缺 `v.reserve()` 那个代码块 |
| `signed-integer-overflow-assumptions` | 中文版缺 `Checked<T>` 那个代码块 |
| `implicit-conversions` | 占位类型名写成中文 `目标类型`，英文版是 `TargetType` |
| `static-initialization-order-fiasco` | 中英是**两篇不同的文章**，英文版按中文版重译 |
| `dangling-string-view` | 中文版缺两节、编号错位、一个代码块漂移 |

**顺带发现工具自己的一个真 bug**：词法分析器把 C++14 数字分隔符
`1'000'000` 里的 `'` 当成字符字面量的开头，于是把后面全部内容（包括注释）
当成字面量吞掉 —— 凡是用了数字分隔符的文档都会被误报"代码不同"。
C++ 库和 Python 脚本都修了，并补了回归测试。

**仓库卫生**：

- `tools/check_stats.py`：重新计算 README 里的每个数字并与仓库实际比对。
  这些数字漂移过好几次，每次都要人工重数才能发现；现在是检查项。
- `producer-consumer-demo` 只有 1 个测试，低于约定的 3 个，补到 3 个。
- `00-overview.md` 漏登记 8 个项目，已补齐，**151 个项目现在全部在册**。
- CONTRIBUTING 里"所有项目都有 include/ + src/"的说法对 6 个扁平布局的小项目
  不成立，改成说明哪些是硬性要求、哪些是小型 demo 的自由选择。
- 新增 PR 模板与 dependabot 配置。
- 示例检查器识别"编译器太旧"（不认识 `-std=c++26`）与"代码坏了"的区别，
  前者跳过、后者失败；CI 优先安装 g++-14 以便真的检查 C++26 的例子。

### 顺带查出的一个子模块问题

`tinyshell` 子模块指向的 commit `687250f` **只在本地存在**：

```
$ git fetch --depth 1 origin 687250fb997684b464290037516fbf6e9f65cd6e
fatal: remote error: upload-pack: not our ref 687250fb997684b464290037516fbf6e9f65cd6e
```

远端 `polarours/tinyshell` 只有 `main` 分支，且只含最早的 "Initial commit"
（LICENSE + README.md）；完整的 shell 实现停在本地从未推送的 `v1` 分支上。
父仓库记录的指针因此指向一个别人拿不到的 commit —— **全新 clone 会拉不到
tinyshell 的代码**。

这会让 CI 的 `submodules: true` 直接失败，所以做了两件事：

1. workflow 里的子模块拉取改为 `continue-on-error`，tinyshell 那个 job 在子模块
   不可用时打印提示后正常结束，而不是让整个 workflow 变红。
2. 链接检查器识别"指向未 checkout 的子模块"的链接，单独报告为**无法验证**，
   而不是误报成坏链。

**这需要仓库作者自己决定**：把 `v1` 推上去（`git push origin v1`），或者把父
仓库的指针改到远端已有的 commit。

## 第十三阶段：项目 README 一致性与代码块完整性（2026-10-22）

**背景**：前几阶段覆盖了 `docs/`、`pitfalls/` 和仓库根目录，但 151 个**项目**
自己的 README 从没被检查过 —— 它们同样有中英两半，同样可能漂移。

**查出的问题**：

| 项目 | 问题 |
|------|------|
| `tiny-http-server` | 中文版是旧稿，缺「当前限制」「试一试」「建议的下一步」三节；**运行章节写的可执行文件是 `http_server`，实际是 `tiny_http_server`**；功能清单还宣称有**中间件**和**静态文件服务**，而项目里根本没有实现 |
| `key-value-store` | 中文版是旧稿，缺协议、选项、限制、建议等 13 节；**运行章节写的 `kv_server` 也不存在**，实际是 `key_value_store` 和 `kv_client` |

两篇的中文版都已按英文版重译，标题数、代码块数、层级完全一致。

**新增两个检查**：

- `tools/check_project_readmes.py` —— 逐对比较 151 个项目 README 的标题骨架、
  代码块数量、中文正文，并核对 README 里 `./build/xxx` 指的可执行文件确实是
  该项目的 CMake 目标。已用注入错误的方式验证它真的能报出来。
- 代码块括号平衡检查 —— 1562 个代码块逐个查花括号/圆括号/方括号是否配平，
  用来发现被截断的示例。

**顺带修掉一个检查流程的漏洞**：括号平衡原先挂在"配对比较"里，一旦某篇文档
漂移到配不上对，就**同时逃过所有检查**（因为配对失败就不再比较）。现在它
独立遍历全部文件，不再依赖配对结果 —— 这正是上面那个注入测试暴露出来的。

## 第十四阶段：中文版路线图索引补全（2026-10-22）

**问题**：`cpp-mastery-roadmap/00-overview.zh-CN.md` 是一份写得不差的**中文导航**，
但它在第四阶段之后就停住了 —— 7 个标题、144 行，只链到 2 个项目。而英文版有
50 个标题、660 行、链到 150 个项目。

也就是说：**中文读者找不到 151 个项目里的 147 个**。而 `00-overview.md` 正是
整个仓库的入口。

**处理**：按英文版的结构把中文版补全 —— 第三/第四阶段的项目清单、C++20 示例
小节，以及第五到第四十三阶段的项目清单，全部译成中文。原文件已有的中文正文
（阶段一到四的叙述、"如何使用这份路线图"、"当前状态说明"）保留不动。

补全后两边的标题数（50 / 50）与项目链接数（150 / 150）完全一致。

**新增检查** `tools/check_overview_parity.py`：比较两份索引的标题骨架与项目
链接集合，任何一边少列一个项目都会被报出来。

## 第十五阶段：示例头注释统一（2026-10-22）

**问题**：120 个示例里有 **32 个**不符合仓库自己的头注释约定 —— 27 个完全没有
头注释，4 个写的是中文，1 个缺编译命令。约定是：

```cpp
// examples/<文件名>.cpp
// <一句话说明这个示例演示什么>
// Compile with: g++ -std=c++NN -o <名字> <文件名>.cpp
```

读者是照着第二行复制命令的，所以**缺了标准号就等于缺了关键信息** ——
`std-flat-map`、`std-expected`、`std-mdspan` 需要 C++23，
`std-format`、`std-jthread`、`std-latch-barrier`、`std-source-location`、
`std-bit-cpp20`、`std-ranges-advanced` 等需要 C++20。照 `c++17` 编会得到一堆
看不懂的报错。

**处理**：给 32 个示例补上符合约定的头注释，标准号是逐个探测出来的
（依次试 c++17 / c++20 / c++23 / c++26，取第一个能编过的）。同时把
44 个 `// Compile:` 统一成 `// Compile with:`。

**顺带修掉一个真实冲突**：`virtual-dispatch-and-slicing.cpp` 和
`virtual-dispatch-example.cpp` 都声明 `-o virtual_dispatch`，在同一个目录里
编译时后者会静默覆盖前者。前者现在输出 `virtual_dispatch_slicing`。

**新增检查**（`tools/check_examples.py`）：

- 每个示例都有符合约定的头注释，且注释是英文的
- 没有两个示例写同一个输出文件名

## 第十六阶段：示例索引（2026-10-22）

**问题**：120 个示例里只有 **16 个**能从文档里点到 —— 剩下的 104 个没有被任何
一篇文档引用。示例是仓库里"能跑"的那一半，却基本没有入口。

**处理**：新增 `examples/README.md` 与 `examples/README.zh-CN.md` 索引，
把 120 个示例按主题分组列出。

索引里的说明文字**直接取自每个示例自己的头注释**（第十五阶段刚统一过），
所以索引不可能和文件头描述得不一样 —— 这是选"从文件生成索引"而不是
"手写索引"的原因。

同时建立"示例 ↔ 路线图项目"的对应：能从文件名唯一确定的（如
`decorator-pattern-demo` → `10-stage10/decorator-pattern`）在索引里附带项目链接。

**新增检查**：索引必须列出每一个示例，新增示例忘了更新索引会被报出来。

## 第十七阶段：文档指向可运行示例（2026-10-22）

**问题**：120 个示例里只有 16 个能从文档里点到。示例是仓库里"能跑"的那一半，
但读者读�� `decorator-pattern.md` 时，看不到能跑的 `decorator-pattern-demo.cpp`。

**处理**：给 **52 篇文档**（104 个中英文件）在收尾章节之前插入一个「Example /
示例」小节，指向对应的可运行示例：

- 36 篇模式文档，示例名与文档名同源（`decorator-pattern` → `decorator-pattern-demo`）
- 16 篇概念文档，示例另取名字（`ownership-and-lifetime` → `smart-pointers-ownership`），
  这部分用显式配对表列出，不靠名字猜

中英两半同时插入，因此标题骨架仍然一一对应。

概念文章与陷阱文章一视同仁：陷阱文章的配对走 `tools/pitfall_pairs.tsv`
显式映射，因此 `object-slicing`、`returning-reference-to-local-object`、
`sfinae-misuse` 三篇也各加上了对应示例的入口。

**结果**：可从文档点到的示例从 16 个增加到 68 个；其余 52 个（标准库与
现代 C++ 的示例）在第十六阶段的 `examples/README.md` 索引里可以找到。
