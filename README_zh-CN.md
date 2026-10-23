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
| 文档 | 134 篇（67 英文 + 67 中文） |
| 陷阱 | 74 篇（37 英文 + 37 中文） |
| 示例 | 120 个可运行示例 |
| 项目 | 151 个项目，横跨 43 个阶段 |
| 测试 | 879（全部通过） |

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
  [深入理解 C++ 对象模型](docs/中文版/深入理解 C++ 对象模型.md)，
  [未定义行为](docs/中文版/未定义行为.md)，
  [C++ 内存模型与原子操作基础](docs/中文版/C++内存模型与原子操作基础.md)，
  [virtual-dispatch-and-slicing.cpp](examples/virtual-dispatch-and-slicing.cpp)

## 路线图总览

[精通路线图](cpp-mastery-roadmap/00-overview.zh-CN.md) 是一个分阶段的学习路径：

| 阶段 | 主题 | 项目数 |
|------|------|--------|
| 1–7 | C++ 基础到高级 | 35 |
| 8–27 | 设计模式（GoF 23 + Specification） | 61 |
| 28–31 | 架构模式 | 12 |
| 32–33 | 并发模式 | 7 |
| 34 | C++ 特有模式（CRTP, SFINAE, 变参模板） | 4 |
| 35 | 内存模式 | 4 |
| 36 | 现代 C++（协程, Ranges, Concepts） | 3 |
| 37 | 错误处理模式 | 4 |
| 38 | 测试模式 | 4 |
| 39 | 内容增强（解析 / Diff 工具） | 3 |
| 40 | C++23 新特性 | 4 |
| 41 | 综合实战（HTTP 框架、路由器、模板引擎） | 3 |
| 42 | 性能模式 | 3 |
| 43 | 工具链模式（CMake, CI/CD, 包管理） | 3 |

## 仓库地图

### 文档

- [docs/en](docs/en) 和 [docs/中文版](docs/中文版)：
  以概念为中心的文章，通常一篇只讲一个核心问题
- [pitfalls/en](pitfalls/en) 和 [pitfalls/中文版](pitfalls/中文版)：
  高频错误、错误假设，以及适合 code review 的风险清单
- 项目 API 参考：
  [术语表](docs/en/glossary.md)、
  [Chat 服务器](docs/en/chat-server-api.md)、
  [Simple 数据库](docs/en/simple-db-api.md)、
  [任务调度器](docs/en/task-scheduler-api.md)、
  [连接池](docs/en/connection-pool-api.md)

### 工具

每项检查查什么、为什么这么查，见 [tools/README.zh-CN.md](tools/README.zh-CN.md)。

- [tools/run_all_checks.sh](tools/run_all_checks.sh)：
  所有仓库检查的统一入口，CI 跑的就是它
- [tools/check_docs.py](tools/check_docs.py)：
  校验每篇文档是否用正确的语言书写，以及中英两半的标题骨架与代码是否一致
- [tools/check_links.py](tools/check_links.py)：
  校验每条相对链接和 `#锚点` 是否仍然有效
- [tools/check_doc_examples.py](tools/check_doc_examples.py)：
  拼装每篇文档的完整示例，编译并运行
- [tools/check_examples.py](tools/check_examples.py)：
  按各文件头注释自己声明的编译命令，编译 `examples/` 下的每个独立示例
- [tools/check_stats.py](tools/check_stats.py)：
  校验 README 里的统计数字是否仍与仓库实际一致
- [tools/build_all_projects.py](tools/build_all_projects.py)：
  配置、构建并测试路线图里的每个项目

```bash
tools/run_all_checks.sh          # 文档检查
tools/run_all_checks.sh --full   # 外加全部项目构建
```

### 示例

- [examples](examples)，索引见 [examples/README.zh-CN.md](examples/README.zh-CN.md)：
  120 个可运行示例，涵盖：
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
- [student-management/v3](cpp-mastery-roadmap/01-beginner/student-management/v3)：
  用 std::string_view 实现借用参数

#### 第二阶段：中级

- [key-value-store](cpp-mastery-roadmap/02-intermediate/key-value-store)：
  完整的键值存储，包含协议、服务器、客户端和并发支持
- [tiny-http-server](cpp-mastery-roadmap/02-intermediate/tiny-http-server)：
  HTTP 服务器，支持路由和线程池
- [copy-move-demo](cpp-mastery-roadmap/02-intermediate/copy-move-demo)：
  可观察的拷贝/移动语义演示
- [reference-binding-demo](cpp-mastery-roadmap/02-intermediate/reference-binding-demo)：
  引用绑定与生命周期延长规则
- [trie](cpp-mastery-roadmap/02-intermediate/trie)：
  前缀树，支持词/前缀计数
- [union-find](cpp-mastery-roadmap/02-intermediate/union-find)：
  并查集，路径压缩 + 按秩合并
- [binary-heap](cpp-mastery-roadmap/02-intermediate/binary-heap)：
  数组实现的二叉堆，自底向上 heapify

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
- [static-init-demo](cpp-mastery-roadmap/03-advanced/static-init-demo)：
  静态初始化顺序难题及其解法
- [skip-list](cpp-mastery-roadmap/03-advanced/skip-list)：
  随机层级的概率型有序集合
- [csv-parser](cpp-mastery-roadmap/03-advanced/csv-parser)：
  RFC-4180 引号规则，支持无损往返
- [base64-codec](cpp-mastery-roadmap/03-advanced/base64-codec)：
  RFC 4648 编解码，严格填充校验

#### 第四阶段：更多练习

- [simple-db](cpp-mastery-roadmap/04-stage4/simple-db)：
  基于文件的键值数据库
- [chat-server](cpp-mastery-roadmap/04-stage4/chat-server)：
  多客户端聊天服务器
- [web-server](cpp-mastery-roadmap/04-stage4/web-server)：
  支持静态文件的 HTTP 服务器
- [mini-redis](cpp-mastery-roadmap/04-stage4/mini-redis)：
  RESP 风格应答与惰性 TTL 的内存 KV 存储
- [task-scheduler](cpp-mastery-roadmap/05-stage5/task-scheduler)：
  优先级/延迟/重复任务调度器
- [timer-wheel](cpp-mastery-roadmap/05-stage5/timer-wheel)：
  O(1) 基于 tick 的时间轮调度
- [blocking-queue](cpp-mastery-roadmap/06-stage6/blocking-queue)：
  双条件变量的有界 MPMC 队列
- [lfu-cache](cpp-mastery-roadmap/07-stage7/lfu-cache)：
  最少使用频率淘汰，LRU 打破平局
- [bloom-filter](cpp-mastery-roadmap/07-stage7/bloom-filter)：
  双哈希布隆过滤器

#### 第八至二十七阶段：设计模式

- [GoF 23 种模式](cpp-mastery-roadmap/00-overview.md#stage-8-projects-08-stage8)：
  全部 23 种 GoF 模式 + Specification
- **命令模式变体**：command-history, command-logger, command-memento, command-router, command-snapshot, command-translator, command-validator
- **观察者模式变体**：observer-bus, observer-buffer, observer-debouncer, observer-filter-chain, observer-mediator, observer-subscription, observer-throttler
- **策略模式变体**：strategy-adapter, strategy-chain, strategy-combinator, strategy-decorator, strategy-evaluator, strategy-factory, strategy-pipeline, strategy-resolver
- [rate-limiter](cpp-mastery-roadmap/08-stage8/rate-limiter)：
  令牌桶、固定窗口、滑动窗口三种限流算法

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
- [actor-mailbox](cpp-mastery-roadmap/33-stage33/actor-mailbox)：
  单消费者 actor 循环的有界邮箱

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
- [type-erasure](cpp-mastery-roadmap/34-stage34/type-erasure)：
  手写 std::function 风格包装（Concept/Holder）
- [allocation-tracker](cpp-mastery-roadmap/35-stage35/allocation-tracker)：
  进程级分配计数与计数分配器
- [error-category](cpp-mastery-roadmap/37-stage37/error-category)：
  映射到 std::errc 的自定义 error_code 类别
- [golden-file-testing](cpp-mastery-roadmap/38-stage38/golden-file-testing)：
  快照断言，逐行 diff 与 UPDATE_GOLDEN 流程

#### 第三十九阶段：内容增强

- [markdown-parser](cpp-mastery-roadmap/39-stage39/markdown-parser)：
  Markdown 分块解析与行内链接提取
- [link-checker](cpp-mastery-roadmap/39-stage39/link-checker)：
  词法路径归一化的相对链接校验
- [text-diff](cpp-mastery-roadmap/39-stage39/text-diff)：
  LCS 逐行 diff，unified 风格渲染

#### 第四十阶段：C++23 新特性

- [std-print](cpp-mastery-roadmap/40-stage40/std-print)：
  格式化输出
- [deducing-this](cpp-mastery-roadmap/40-stage40/deducing-this)：
  C++23 显式对象参数
- [zip-range](cpp-mastery-roadmap/40-stage40/zip-range)：
  并行迭代
- [generator-pipeline](cpp-mastery-roadmap/40-stage40/generator-pipeline)：
  惰性 std::generator 流水线，早退出向上传播

#### 第四十一阶段：综合实战

- [mini-http-framework](cpp-mastery-roadmap/41-stage41/mini-http-framework)：
  组合 10 种设计模式的完整 HTTP 框架
- [http-router](cpp-mastery-roadmap/41-stage41/http-router)：
  方法+路径路由，提取 :param 参数
- [template-engine](cpp-mastery-roadmap/41-stage41/template-engine)：
  mustache 风格段落与上下文链

#### 第四十二阶段：性能模式

- [cache-friendly](cpp-mastery-roadmap/42-stage42/cache-friendly)：
  AoS 与 SoA 数据布局的缓存性能
- [simd-pattern](cpp-mastery-roadmap/42-stage42/simd-pattern)：
  手动 SIMD 风格向量化
- [memory-alignment](cpp-mastery-roadmap/42-stage42/memory-alignment)：
  缓存行对齐和对齐分配

#### 第四十三阶段：工具链模式

- [cmake-pattern](cpp-mastery-roadmap/43-stage43/cmake-pattern)：
  程序化 CMakeLists.txt 生成
- [package-manager](cpp-mastery-roadmap/43-stage43/package-manager)：
  依赖解析和包管理
- [ci-cd-pattern](cpp-mastery-roadmap/43-stage43/ci-cd-pattern)：
  CI/CD 管道模式

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
