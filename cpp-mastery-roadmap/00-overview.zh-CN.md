[中文](00-overview.zh-CN.md) | [English](00-overview.md)

# C++ Mastery Roadmap

这份路线图的目的，是把仓库里的文档、pitfalls、examples 和项目练习
串成一条可执行的学习路径。

它不是一份“面面俱到”的教材目录，
而是一份帮助你在概念、代码和工程实践之间来回切换的导航。

## 如何使用这份路线图

- 每个阶段都按四个维度来推进：
  核心主题、常见陷阱、示例代码、动手练习
- 不要试图线性读完整个仓库
- 更好的节奏是：
  学一个概念，看一个失败案例，再去改一段代码
- 路线图里的部分项目本来就是未完成状态
  重点不是“阅读成品”，而是“在理解后继续扩展”

## 第一阶段：基础语义

重点：
所有权、生命周期、RAII、`const`、未定义行为。

建议阅读：

- [所有权：从这里开始](../docs/中文版/所有权-从这里开始.md)
- [所有权与生命周期](../docs/中文版/所有权与生命周期.md)
- [RAII与异常安全](../docs/中文版/RAII与异常安全.md)
- [const 的真实语义](../docs/中文版/const 的真实语义.md)
- [未定义行为](../docs/中文版/未定义行为.md)

对应 pitfalls：

- [悬垂指针](../pitfalls/中文版/悬垂指针.md)
- [返回局部对象引用](../pitfalls/中文版/返回局部对象引用.md)
- [析构函数抛出异常](../pitfalls/中文版/析构函数抛出异常.md)

对应示例：

- [ownership-and-lifetime.cpp](../examples/ownership-and-lifetime.cpp)
- [const-vs-non-const.cpp](../examples/const-vs-non-const.cpp)

动手方向：

- 通读 [student-management/v1](01-beginner/student-management/v1)
- 至少改一个点：
  输入校验、文件格式设计、UI 与业务逻辑拆分，或者错误处理
- 参考 [student-management/v2](01-beginner/student-management/v2) 中的改进示例

## 第二阶段：引用、move 与值类别

重点：
引用绑定、move 语义、转发、拷贝消除，以及接口设计中的值语义。

建议阅读：

- [从值类别到引用绑定](../docs/中文版/从值类别到引用绑定.md)
- [左值引用](../docs/中文版/左值引用.md)
- [右值引用](../docs/中文版/右值引用.md)
- [左值引用和右值引用](../docs/中文版/左值引用和右值引用.md)
- [深入理解 move 语义](../docs/中文版/深入理解 move 语义.md)
- [转发引用与完美转发](../docs/中文版/转发引用与完美转发.md)
- [拷贝消除与prvalue实体化](../docs/中文版/拷贝消除与prvalue实体化.md)
- [noexcept与移动操作](../docs/中文版/noexcept与移动操作.md)

对应 pitfalls：

- [对const对象使用move](../pitfalls/中文版/对const对象使用move.md)
- [对象切片](../pitfalls/中文版/对象切片.md)
- [隐式转换](../pitfalls/中文版/隐式转换.md)

对应示例：

- [move-semantics-example.cpp](../examples/move-semantics-example.cpp)
- [copy_on_write_example.cpp](../examples/copy_on_write_example.cpp)

动手方向：

- 回过头重构 beginner 项目
- 把接口里的“借用 / 拥有 / 转移所有权”边界表达得更清楚

## 第三阶段：对象模型、并发与深层语义

重点：
虚函数分发、模板实例化、内存模型、ABI 边界、协程生命周期。

建议阅读：

- [虚函数分发与对象模型](../docs/中文版/虚函数分发与对象模型.md)
- [C++内存模型与原子操作基础](../docs/中文版/C++内存模型与原子操作基础.md)
- [模板实例化与两阶段查找](../docs/中文版/模板实例化与两阶段查找.md)
- [PImpl与ABI稳定性](../docs/中文版/PImpl与ABI稳定性.md)
- [协程的生命周期与挂起点](../docs/中文版/协程的生命周期与挂起点.md)

对应 pitfalls：

- [双重检查锁定失效](../pitfalls/中文版/双重检查锁定失效.md)
- [线程安全的迷思](../pitfalls/中文版/线程安全的迷思.md)
- [迭代器失效](../pitfalls/中文版/迭代器失效.md)
- [静态初始化顺序问题](../pitfalls/中文版/静态初始化顺序问题.md)

动手方向：

- 任选一个 pitfall，先写出最小复现
- 再用更合理的所有权模型或同步方式重写它

## 第四阶段：做更大的东西

重点：
把语义理解真正落到设计决策和工程代码上。

建议先看：

- [02 Intermediate 概览](02-intermediate/README.zh-CN.md)

项目入口：

- [tiny-http-server](02-intermediate/tiny-http-server/README.md)：
  socket、请求解析、路由、响应构造、固定大小线程池
- [key-value-store](02-intermediate/key-value-store/README.md)：
  处于设计阶段的存储练习项目，重点在状态所有权与持久化边界
- [tinyshell](../tinyshell/README.md)：
  练习命令解析、进程行为和系统接口

建议顺序：

1. 先读阶段概览，明确项目的学习目标和非目标
2. 再把当前项目跑起来，读清楚现状
3. 列出它的所有权边界、生命周期假设和错误路径
4. 补上一块缺失内容：
   测试、更完整的解析、RAII 封装、更清晰的接口、持久化规则，或与实现一致的文档

## 当前状态说明

这份路线图是持续更新的导航，不是已经完工的课程。
有些部分已经比较成型，有些项目还处在早期阶段，
仓库里也确实存在“设计意图”和“实际实现”之间的空隙。

这不是问题本身。
把这些空隙看见，然后有意识地补上它们，本来就是这个项目的一部分。
