[中文](README_zh-CN.md) | [English](README.md)

# Cppuccino

Cppuccino 是一个整理中的 C++ 学习仓库。
它把概念文档、常见陷阱、可运行示例和项目式练习放在同一个地方，
尽量把“理解语义”和“亲手写代码”连起来。

你可以把它看作一份个人学习记录，也可以把它当作一个逐步成型的公开学习资源。

> 本项目反映的是作者当前阶段对 C++ 的理解。
> 内容仍在持续演进，欢迎指正、补充和讨论。

## 从这里开始

如果你是第一次来到这个仓库，建议按这个顺序进入：

1. 先看 [路线图概览](cpp-mastery-roadmap/00-overview.zh-CN.md)。
2. 选一个概念主题开始阅读。
3. 配套读一篇相关 pitfalls。
4. 最后到 example 或小项目里动手。

### 建议入口

- 所有权与生命周期：
  [所有权：从这里开始](docs/中文版/所有权-从这里开始.md)，
  [所有权与生命周期](docs/中文版/所有权与生命周期.md)，
  [RAII 与异常安全](docs/中文版/RAII与异常安全.md)，
  [悬垂指针](pitfalls/中文版/悬垂指针.md)，
  [`string_view` 悬垂问题](pitfalls/中文版/string-view-悬垂问题.md)，
  [ownership-and-lifetime.cpp](examples/ownership-and-lifetime.cpp)
- 引用、move 语义和值类别：
  [左值引用和右值引用](docs/中文版/左值引用和右值引用.md)，
  [深入理解 move 语义](docs/中文版/深入理解 move 语义.md)，
  [转发引用与完美转发](docs/中文版/转发引用与完美转发.md)，
  [对 `const` 对象使用 move](pitfalls/中文版/对const对象使用move.md)，
  [move-semantics-example.cpp](examples/move-semantics-example.cpp)
- 对象模型与更深层语义：
  [虚函数分发与对象模型](docs/中文版/虚函数分发与对象模型.md)，
  [未定义行为](docs/中文版/未定义行为.md)，
  [C++ 内存模型与原子操作基础](docs/中文版/C++内存模型与原子操作基础.md)，
  [静态初始化顺序问题](pitfalls/中文版/静态初始化顺序问题.md)，
  [双重检查锁定失效](pitfalls/中文版/双重检查锁定失效.md)

## 仓库地图

- [docs/en](docs/en) 和 [docs/中文版](docs/中文版)：
  以概念为中心的文章，通常一篇只讲一个核心问题
- [pitfalls/en](pitfalls/en) 和 [pitfalls/中文版](pitfalls/中文版)：
  高频错误、错误假设，以及适合 code review 的风险清单
- [examples](examples)：
  与部分主题对应的小型示例代码
- [cpp-mastery-roadmap](cpp-mastery-roadmap)：
  从基础练习到系统项目的阶段式学习路径
- [tinyshell](tinyshell)：
  额外的系统编程实验项目

## 如何使用这个仓库

- 如果你想快速弄懂一个概念，就从 `docs` 按主题阅读。
- 如果你想按阶段推进，就从 `cpp-mastery-roadmap` 进入。
- 如果你在写代码或做 review，就把 `pitfalls` 当成检查表。
- 如果你想把抽象语义和代码放在一起理解，就在文章和 `examples` 之间来回跳。

## 项目取向

- 一篇文档尽量只聚焦一个核心思想。
- 内容尽量简洁，但必须有技术依据。
- 优先解释原因，而不是堆砌结论。
- 比起经验口号，更关注所有权、生命周期、语义和工程权衡。

## 贡献

详细说明见 [CONTRIBUTING.md](CONTRIBUTING.md)。

特别欢迎这几类贡献：技术纠错、表述简化、更好的示例、严谨的反例与讨论。
