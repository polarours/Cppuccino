[中文](README_zh-CN.md) | [English](README.md)

# Cppuccino

Cppuccino 是一个整理中的 C++ 双语学习仓库。
它把概念文档、常见陷阱、可运行示例和项目式练习放在同一个地方，
尽量把"理解语义"和"亲手写代码"连起来。

你可以把它看作一份个人学习记录，也可以把它当作一个逐步成型的公开学习资源。

> 本项目反映的是作者当前阶段对 C++ 的理解。
> 内容仍在持续演进，欢迎指正、补充和讨论。

## 快速统计

| 类别 | 数量 |
|------|------|
| 文档 | 42 篇（20 英文 + 22 中文） |
| 陷阱 | 34 篇（17 英文 + 17 中文） |
| 示例 | 47 个可运行示例 |
| 项目 | 115 个项目，横跨 40 个阶段 |
| 测试 | 578（全部通过） |

## 从这里开始

如果你是第一次来到这个仓库，建议按这个顺序进入：

1. 先看 [路线图概览](cpp-mastery-roadmap/00-overview.zh-CN.md)。
2. 选一个概念主题开始阅读。
3. 配套读一篇相关 pitfalls。
4. 最后到 example 或小项目里动手。

### 建议入口

- **所有权与生命周期：**
  [所有权：从这里开始](docs/中文版/所有权-从这里开始.md)，
  [所有权与生命周期](docs/中文版/所有权与生命周期.md)，
  [RAII 与异常安全](docs/中文版/RAII与异常安全.md)，
  [悬垂指针](pitfalls/中文版/悬垂指针.md)，
  [ownership-and-lifetime.cpp](examples/ownership-and-lifetime.cpp)

- **引用、move 语义和值类别：**
  [左值引用和右值引用](docs/中文版/左值引用和右值引用.md)，
  [深入理解 move 语义](docs/中文版/深入理解 move 语义.md)，
  [转发引用与完美转发](docs/中文版/转发引用与完美转发.md)，
  [move-semantics-example.cpp](examples/move-semantics-example.cpp)

- **对象模型与更深层语义：**
  [虚函数分发与对象模型](docs/中文版/虚函数分发与对象模型.md)，
  [未定义行为](docs/中文版/未定义行为.md)，
  [C++ 内存模型与原子操作基础](docs/中文版/C++内存模型与原子操作基础.md)，
  [virtual-dispatch-and-slicing.cpp](examples/virtual-dispatch-and-slicing.cpp)

## 路线图总览

[精通路线图](cpp-mastery-roadmap/00-overview.zh-CN.md) 是一个分阶段的学习路径：

| 阶段 | 主题 | 项目数 |
|------|------|--------|
| 1–7 | C++ 基础到高级 | 7 |
| 8–27 | 设计模式（GoF 23 + Specification） | 60 |
| 28–31 | 架构模式 | 12 |
| 32–33 | 并发模式 | 6 |
| 34 | C++ 特有模式（CRTP, SFINAE, 变参模板） | 3 |
| 35 | 内存模式 | 3 |
| 36 | 现代 C++（协程, Ranges, Concepts） | 3 |
| 37 | 错误处理模式 | 3 |
| 38 | 测试模式 | 3 |
| 39 | 补充示例和陷阱 | — |
| 40 | C++23 新特性 | 3 |

## 仓库地图

### 文档

- [docs/en](docs/en) 和 [docs/中文版](docs/中文版)：
  以概念为中心的文章，通常一篇只讲一个核心问题
- [pitfalls/en](pitfalls/en) 和 [pitfalls/中文版](pitfalls/中文版)：
  高频错误、错误假设，以及适合 code review 的风险清单

### 示例

- [examples](examples)：
  47 个可运行示例，涵盖：
  - 核心概念：所有权、move 语义、智能指针、RAII
  - 现代 C++：lambda、constexpr、结构化绑定、ranges
  - C++20：concepts、ranges、span、format、coroutines
  - C++23：fold expressions、CRTP、SFINAE、type traits
  - 陷阱复现：悬垂引用、对象切片、迭代器失效

### 项目

#### 第一阶段：入门

- [student-management/v1](cpp-mastery-roadmap/01-beginner/student-management/v1)：
  基础学生管理系统
- [student-management/v2](cpp-mastery-roadmap/01-beginner/student-management/v2)：
  改进版本，更好的 I/O 和测试

#### 第二阶段：中级

- [key-value-store](cpp-mastery-roadmap/02-intermediate/key-value-store)：
  完整的键值存储，包含协议、服务器、客户端和并发支持
- [tiny-http-server](cpp-mastery-roadmap/02-intermediate/tiny-http-server)：
  HTTP 服务器，支持路由和线程池

#### 第三阶段：高级

- [thread-pool](cpp-mastery-roadmap/03-advanced/thread-pool)：
  线程池，支持 futures 和同步
- [allocator](cpp-mastery-roadmap/03-advanced/allocator)：
  固定块内存分配器，带 STL 接口
- [json-parser](cpp-mastery-roadmap/03-advanced/json-parser)：
  递归下降 JSON 解析器
- [http-parser](cpp-mastery-roadmap/03-advanced/http-parser)：
  HTTP 请求/响应解析器
- [smart-pointer](cpp-mastery-roadmap/03-advanced/smart-pointer)：
  自定义智能指针实现
- [event-loop](cpp-mastery-roadmap/03-advanced/event-loop)：
  带定时器优先队列的事件循环

#### 第四阶段：更多练习

- [simple-db](cpp-mastery-roadmap/04-stage4/simple-db)：
  基于文件的键值数据库
- [chat-server](cpp-mastery-roadmap/04-stage4/chat-server)：
  多客户端聊天服务器
- [web-server](cpp-mastery-roadmap/04-stage4/web-server)：
  支持静态文件的 HTTP 服务器

#### 第八至二十七阶段：设计模式

- [GoF 23 种模式](cpp-mastery-roadmap/00-overview.md#stage-8-projects-8-stage8)：
  全部 23 种 GoF 模式 + Specification
- **命令模式变体**：command-history, command-logger, command-memento, command-router, command-snapshot, command-translator, command-validator
- **观察者模式变体**：observer-bus, observer-buffer, observer-debouncer, observer-filter-chain, observer-mediator, observer-subscription, observer-throttler
- **策略模式变体**：strategy-adapter, strategy-chain, strategy-combinator, strategy-decorator, strategy-evaluator, strategy-factory, strategy-pipeline, strategy-resolver

#### 第二十八至三十一阶段：架构模式

- [mvc-pattern](cpp-mastery-roadmap/28-stage28/mvc-pattern)：
  Model-View-Controller 架构
- [repository-pattern](cpp-mastery-roadmap/28-stage28/repository-pattern)：
  数据访问抽象
- [cqrs-pattern](cpp-mastery-roadmap/28-stage28/cqrs-pattern)：
  命令查询职责分离
- [event-sourcing](cpp-mastery-roadmap/29-stage29/event-sourcing)：
  基于事件的状态管理
- [circuit-breaker](cpp-mastery-roadmap/29-stage29/circuit-breaker)：
  断路器，容错模式
- [middleware-pipeline](cpp-mastery-roadmap/29-stage29/middleware-pipeline)：
  链式请求/响应处理
- [service-locator](cpp-mastery-roadmap/30-stage30/service-locator)：
  服务定位器
- [unit-of-work](cpp-mastery-roadmap/30-stage30/unit-of-work)：
  工作单元
- [active-record](cpp-mastery-roadmap/30-stage30/active-record)：
  活动记录
- [type-object](cpp-mastery-roadmap/31-stage31/type-object)：
  类型对象
- [delegation](cpp-mastery-roadmap/31-stage31/delegation)：
  委托模式
- [balking](cpp-mastery-roadmap/31-stage31/balking)：
  省略模式

#### 第三十二至三十三阶段：并发模式

- [guarded-suspension](cpp-mastery-roadmap/32-stage32/guarded-suspension)：
  保护性暂停
- [producer-consumer](cpp-mastery-roadmap/32-stage32/producer-consumer)：
  生产者消费者
- [double-buffer](cpp-mastery-roadmap/32-stage32/double-buffer)：
  双缓冲
- [read-write-lock](cpp-mastery-roadmap/33-stage33/read-write-lock)：
  读写锁
- [lock-free-queue](cpp-mastery-roadmap/33-stage33/lock-free-queue)：
  无锁队列
- [thread-pool-advanced](cpp-mastery-roadmap/33-stage33/thread-pool-advanced)：
  高级线程池

#### 第三十四至三十八阶段

- [crtp-pattern](cpp-mastery-roadmap/34-stage34/crtp-pattern)：
  CRTP 模式
- [sfinae-pattern](cpp-mastery-roadmap/34-stage34/sfinae-pattern)：
  SFINAE 模式
- [varargs-pattern](cpp-mastery-roadmap/34-stage34/varargs-pattern)：
  变参模板模式
- [memory-pool](cpp-mastery-roadmap/35-stage35/memory-pool)：
  内存池
- [arena-allocator](cpp-mastery-roadmap/35-stage35/arena-allocator)：
  Arena 分配器
- [slab-allocator](cpp-mastery-roadmap/35-stage35/slab-allocator)：
  Slab 分配器
- [coroutine-pattern](cpp-mastery-roadmap/36-stage36/coroutine-pattern)：
  C++20 协程
- [ranges-pattern](cpp-mastery-roadmap/36-stage36/ranges-pattern)：
  Ranges 模式
- [concepts-pattern](cpp-mastery-roadmap/36-stage36/concepts-pattern)：
  C++20 Concepts
- [result-type](cpp-mastery-roadmap/37-stage37/result-type)：
  Result 类型
- [error-code](cpp-mastery-roadmap/37-stage37/error-code)：
  错误码模式
- [expected-pattern](cpp-mastery-roadmap/37-stage37/expected-pattern)：
  Expected 模式
- [mock-objects](cpp-mastery-roadmap/38-stage38/mock-objects)：
  Mock 对象
- [test-fixture](cpp-mastery-roadmap/38-stage38/test-fixture)：
  测试夹具
- [parameterized-tests](cpp-mastery-roadmap/38-stage38/parameterized-tests)：
  参数化测试

#### 第四十阶段：C++23 新特性

- [std-print](cpp-mastery-roadmap/40-stage40/std-print)：
  格式化输出
- [deducing-this](cpp-mastery-roadmap/40-stage40/deducing-this)：
  C++23 显式对象参数
- [zip-range](cpp-mastery-roadmap/40-stage40/zip-range)：
  并行迭代

### 其他

- [tinyshell](tinyshell)：
  支持作业控制、信号处理和变量展开的 Unix shell

## 如何使用这个仓库

- **按主题阅读**，如果你想快速弄懂一个 C++ 概念。
- **按路线图阅读**，如果你想按阶段推进。
- **把 pitfalls 当成检查表**，在写代码或做 review 时使用。
- **在文档和示例之间跳转**，如果你想把语义解释和代码放在一起理解。
- **构建项目**，在实际代码中练习应用概念。

## 构建项目

大多数项目使用 CMake。构建和测试任何项目：

```bash
cd cpp-mastery-roadmap/<阶段>/<项目>
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

部分项目需要 C++20 或 C++23 — 请查看 CMakeLists.txt 中的 `CMAKE_CXX_STANDARD`。

## 项目取向

- 一篇文档尽量只聚焦一个核心思想。
- 内容尽量简洁，但必须有技术依据。
- 优先解释原因，而不是堆砌结论。
- 比起经验口号，更关注所有权、生命周期、语义和工程权衡。

## 贡献

详细说明见 [CONTRIBUTING.md](CONTRIBUTING.md)。

特别欢迎这几类贡献：技术纠错、表述简化、更好的示例、严谨的反例与讨论。
