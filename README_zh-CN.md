[中文](README_zh-CN.md) | [English](README.md)

# Cppuccino

Cppuccino 是一个整理中的 C++ 学习仓库。
它把概念文档、常见陷阱、可运行示例和项目式练习放在同一个地方，
尽量把"理解语义"和"亲手写代码"连起来。

你可以把它看作一份个人学习记录，也可以把它当作一个逐步成型的公开学习资源。

> 本项目反映的是作者当前阶段对 C++ 的理解。
> 内容仍在持续演进，欢迎指正、补充和讨论。

## 快速统计

| 类别 | 数量 |
|------|------|
| 文档 | 41 篇（20 英文 + 21 中文） |
| 陷阱 | 30 篇（15 英文 + 15 中文） |
| 示例 | 34 个可运行示例 |
| 项目 | 4 个阶段（入门 → 高级） |
| 测试 | 165+（全部通过） |
| 术语表 | 100+ 术语（英文 + 中文） |

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

## 仓库地图

### 文档

- [docs/en](docs/en) 和 [docs/中文版](docs/中文版)：
  以概念为中心的文章，通常一篇只讲一个核心问题
- [pitfalls/en](pitfalls/en) 和 [pitfalls/中文版](pitfalls/中文版)：
  高频错误、错误假设，以及适合 code review 的风险清单
- [术语表](docs/glossary.md) / [术语表](docs/中文版/术语表.md)：
  C++ 术语快速参考

### 示例

- [examples](examples)：
  34 个可运行示例，涵盖：
  - 核心概念：所有权、move 语义、智能指针
  - 现代 C++：lambda、constexpr、结构化绑定、ranges
  - C++20：concepts、ranges、span、format
  - 陷阱复现：悬垂引用、对象切片

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
- [tinyshell](tinyshell)：
  Unix shell，支持作业控制和信号处理

#### 第三阶段：高级

- [thread-pool](cpp-mastery-roadmap/03-advanced/thread-pool)：
  线程池，支持 futures 和同步
- [allocator](cpp-mastery-roadmap/03-advanced/allocator)：
  固定块内存分配器，带 STL 接口
- [json-parser](cpp-mastery-roadmap/03-advanced/json-parser)：
  递归下降 JSON 解析器
- [http-parser](cpp-mastery-roadmap/03-advanced/http-parser)：
  HTTP 请求/响应解析器

## 如何使用这个仓库

- **按主题阅读**，如果你想快速弄懂一个 C++ 概念。
- **按路线图阅读**，如果你想按阶段推进。
- **把 pitfalls 当成检查表**，在写代码或做 review 时使用。
- **在文档和示例之间跳转**，如果你想把语义解释和代码放在一起理解。
- **构建项目**，在实际代码中练习应用概念。

## 项目取向

- 一篇文档尽量只聚焦一个核心思想。
- 内容尽量简洁，但必须有技术依据。
- 优先解释原因，而不是堆砌结论。
- 比起经验口号，更关注所有权、生命周期、语义和工程权衡。

## 贡献

详细说明见 [CONTRIBUTING.md](CONTRIBUTING.md)。

特别欢迎这几类贡献：技术纠错、表述简化、更好的示例、严谨的反例与讨论。
