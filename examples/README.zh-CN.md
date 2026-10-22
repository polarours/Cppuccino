# 示例

每个示例都是一个自包含的单文件。文件头注释里写明了它演示什么、
以及怎么编译。

```bash
cd examples
g++ -std=c++20 -o demo demo-file.cpp && ./demo
```

## 核心概念

- [abstract-factory-demo.cpp](abstract-factory-demo.cpp) — 演示抽象工厂模式：不指定具体类，创建一族相互关联的对象。
  - 属于 [abstract-factory](../cpp-mastery-roadmap/15-stage15/abstract-factory/README.md)
- [active-record-demo.cpp](active-record-demo.cpp) — 演示 Active Record 模式：每个对象都知道如何持久化自身（对象 + 数据行 + 数据访问集于一体）。
  - 属于 [active-record](../cpp-mastery-roadmap/30-stage30/active-record/README.md)
- [adapter-pattern-demo.cpp](adapter-pattern-demo.cpp) — 演示适配器模式：把 VlcPlayer + Mp3Player 适配到 MediaPlayer 接口。
  - 属于 [adapter-pattern](../cpp-mastery-roadmap/10-stage10/adapter-pattern/README.md)
- [auto-decltype.cpp](auto-decltype.cpp) — 演示 C++ 中的 auto、decltype 与 decltype(auto)。
- [balking-demo.cpp](balking-demo.cpp) — 演示 Balking 模式：只有对象处于正确状态时操作才继续，否则立即放弃。
  - 属于 [balking](../cpp-mastery-roadmap/31-stage31/balking/README.md)
- [bridge-pattern-demo.cpp](bridge-pattern-demo.cpp) — 演示桥接模式：让 Abstraction 与 Implementation 层次结构解耦。
  - 属于 [bridge-pattern](../cpp-mastery-roadmap/27-stage27/bridge-pattern/README.md)
- [builder-demo.cpp](builder-demo.cpp) — 演示建造者模式：分步骤构建复杂对象，并用 director 处理常见配置。
- [chain-of-responsibility-demo.cpp](chain-of-responsibility-demo.cpp) — 演示责任链模式：Auth -> Logging -> Validation -> Processing。
  - 属于 [chain-of-responsibility](../cpp-mastery-roadmap/12-stage12/chain-of-responsibility/README.md)
- [chrono-calendar.cpp](chrono-calendar.cpp) — 演示 C++20 chrono 日历：年/月/日运算、星期查询与日期差值（不依赖第三方日期库）。
- [command-pattern-demo.cpp](command-pattern-demo.cpp) — 演示命令模式：TextEditor、InsertCommand 与 CommandHistory（撤销/重做）。
  - 属于 [command-pattern](../cpp-mastery-roadmap/09-stage9/command-pattern/README.md)
- [composite-demo.cpp](composite-demo.cpp) — 演示组合模式：统一对待单个对象与对象组合（文件系统树）。
- [connection-pool-concurrency.cpp](connection-pool-concurrency.cpp) — 演示 ConnectionPool 的并发访问与线程安全。
  - 属于 [connection-pool](../cpp-mastery-roadmap/06-stage6/connection-pool/README.md)
- [const-vs-non-const.cpp](const-vs-non-const.cpp) — 演示 const 与非 const 方法之间的性能差异。
- [constexpr.cpp](constexpr.cpp) — 演示 constexpr 变量、函数与 if constexpr。
- [copy-elision.cpp](copy-elision.cpp) — 演示 C++ 中的拷贝省略与 prvalue 实化。
- [copy_on_write_example.cpp](copy_on_write_example.cpp) — 演示 C++ 中的 Copy-on-Write (COW) 模式。
- [coroutine-basics.cpp](coroutine-basics.cpp) — 演示 C++20 协程基础。
- [cqrs-pattern-demo.cpp](cqrs-pattern-demo.cpp) — 演示 CQRS 模式：CommandBus 与 QueryBus 分离，AccountService 搭配内存存储。
  - 属于 [cqrs-pattern](../cpp-mastery-roadmap/28-stage28/cqrs-pattern/README.md)
- [crtp-basics.cpp](crtp-basics.cpp) — 演示用 CRTP（Curiously Recurring Template Pattern）实现静态多态。
- [decorator-pattern-demo.cpp](decorator-pattern-demo.cpp) — 演示装饰器模式：FileDataSource 加上 Encryption、Compression 与 Logging。
  - 属于 [decorator-pattern](../cpp-mastery-roadmap/10-stage10/decorator-pattern/README.md)
- [deducing-this.cpp](deducing-this.cpp) — 演示 C++26 显式对象参数（“deducing this”）：用一份成员函数定义覆盖 ref-qualified 重载与 CRTP 风格的链式接口，无需重复代码。
  - 属于 [deducing-this](../cpp-mastery-roadmap/40-stage40/deducing-this/README.md)
- [delegation-demo.cpp](delegation-demo.cpp) — 演示委托模式：对象把部分工作转交给辅助对象，而不是自己实现全部逻辑。
  - 属于 [delegation](../cpp-mastery-roadmap/31-stage31/delegation/README.md)
- [event-sourcing-demo.cpp](event-sourcing-demo.cpp) — 演示 Event Sourcing：EventStore、Account 聚合与基于事件的回放。
  - 属于 [event-sourcing](../cpp-mastery-roadmap/29-stage29/event-sourcing/README.md)
- [facade-demo.cpp](facade-demo.cpp) — 演示外观模式：为一系列复杂子系统提供简化接口（家庭影院开机流程）。
- [factory-pattern-demo.cpp](factory-pattern-demo.cpp) — 演示工厂方法模式与自动注册机制。
  - 属于 [factory-pattern](../cpp-mastery-roadmap/10-stage10/factory-pattern/README.md)
- [flyweight-pattern-demo.cpp](flyweight-pattern-demo.cpp) — 演示享元模式：文本编辑器中的共享字体。
  - 属于 [flyweight-pattern](../cpp-mastery-roadmap/12-stage12/flyweight-pattern/README.md)
- [fold-expressions.cpp](fold-expressions.cpp) — 演示用 C++17 折叠表达式做求和、相乘与连接。
- [forwarding-references.cpp](forwarding-references.cpp) — 演示 C++ 中的转发引用与完美转发。
- [if-consteval.cpp](if-consteval.cpp) — 演示 C++23 if consteval：按求值是否发生在编译期来分支（取代 std::is_constant_evaluated() 惯用法）。
- [inplace-vector.cpp](inplace-vector.cpp) — 演示 std::inplace_vector (C++26)：带内联存储的 vector，无堆分配、容量固定，接口与 std::vector 相同。
- [interpreter-pattern-demo.cpp](interpreter-pattern-demo.cpp) — 演示解释器模式：由 Number/Add/Multiply/Variable 组成的表达式树。
  - 属于 [interpreter-pattern](../cpp-mastery-roadmap/14-stage14/interpreter-pattern/README.md)
- [lambda-expressions.cpp](lambda-expressions.cpp) — 演示 C++11/14/17 中的 lambda 表达式。
- [lvalue-rvalue-references.cpp](lvalue-rvalue-references.cpp) — 演示 C++ 中的左值引用与右值引用。
- [mediator-pattern-demo.cpp](mediator-pattern-demo.cpp) — 演示中介者模式：ChatRoom 在多个 User 之间居中协调。
  - 属于 [mediator-pattern](../cpp-mastery-roadmap/11-stage11/mediator-pattern/README.md)
- [memento-pattern-demo.cpp](memento-pattern-demo.cpp) — 演示备忘录模式：用 Editor + History 实现撤销/重做。
  - 属于 [memento-pattern](../cpp-mastery-roadmap/13-stage13/memento-pattern/README.md)
- [move-semantics-advanced.cpp](move-semantics-advanced.cpp) — 演示进阶移动语义：NRVO、移动与复制的区别，以及常见陷阱。
- [move-semantics-example.cpp](move-semantics-example.cpp) — 演示移动语义，以及复制与移动有何不同。
- [mvc-pattern-demo.cpp](mvc-pattern-demo.cpp) — 演示 MVC 模式：TodoModel / TodoView / TodoController 之间的配合。
  - 属于 [mvc-pattern](../cpp-mastery-roadmap/28-stage28/mvc-pattern/README.md)
- [noexcept-move.cpp](noexcept-move.cpp) — 演示 C++ 中的 noexcept 与移动操作。
- [nttp-class-type.cpp](nttp-class-type.cpp) — 演示 C++20 的类类型非类型模板参数与 `template<auto>`，把编译期值当作类型写进模板实参。
- [observer-pattern-demo.cpp](observer-pattern-demo.cpp) — 演示 Subject-Observer：Logger 与 Counter 充当观察者。
  - 属于 [observer-pattern](../cpp-mastery-roadmap/09-stage9/observer-pattern/README.md)
- [ownership-and-lifetime.cpp](ownership-and-lifetime.cpp) — 演示 C++17 中不同的所有权语义及其对对象生命周期的影响。
- [perfect-forwarding.cpp](perfect-forwarding.cpp) — 演示转发引用与完美转发。
- [pimpl-idiom.cpp](pimpl-idiom.cpp) — 演示用于保持 ABI 稳定的 PImpl（Pointer to Implementation）惯用法。
- [prototype-demo.cpp](prototype-demo.cpp) — 演示原型模式：克隆已有对象，而不是从头重建（带附件的文档模板）。
- [proxy-demo.cpp](proxy-demo.cpp) — 演示代理模式：用代理对象控制对真实对象的访问（延迟加载 + 访问日志）。
- [raii-and-exception-safety.cpp](raii-and-exception-safety.cpp) — 演示 C++ 中的 RAII 模式与异常安全。
- [range-based-for.cpp](range-based-for.cpp) — 演示基于范围的 for 循环在各类容器上的用法。
- [repository-pattern-demo.cpp](repository-pattern-demo.cpp) — 演示仓储模式：User 实体与内存仓储。
  - 属于 [repository-pattern](../cpp-mastery-roadmap/28-stage28/repository-pattern/README.md)
- [rule-of-zero-three-five.cpp](rule-of-zero-three-five.cpp) — 演示 C++ 中的零法则、三法则与五法则。
- [service-locator-demo.cpp](service-locator-demo.cpp) — 演示服务定位器模式：由中央注册表向客户端提供共享服务（不硬编码依赖）。
  - 属于 [service-locator](../cpp-mastery-roadmap/30-stage30/service-locator/README.md)
- [sfinae-type-traits.cpp](sfinae-type-traits.cpp) — 演示基于 SFINAE 的类型特征与自定义检测惯用法。
- [singleton-pattern-demo.cpp](singleton-pattern-demo.cpp) — 演示 Meyers 单例以及线程安全的 Logger/Config 单例。
  - 属于 [singleton-pattern](../cpp-mastery-roadmap/13-stage13/singleton-pattern/README.md)
- [smart-pointer-shared.cpp](smart-pointer-shared.cpp) — 演示用 std::shared_ptr 实现共享所有权。
- [smart-pointer-unique.cpp](smart-pointer-unique.cpp) — 演示用 std::unique_ptr 实现独占所有权。
- [smart-pointers-ownership.cpp](smart-pointers-ownership.cpp) — 演示 unique_ptr、shared_ptr、weak_ptr 与各种所有权模式。
- [spaceship-operator.cpp](spaceship-operator.cpp) — 演示 C++20 三路比较 (operator<=>)：默认字典序、自定义排序与比较运算的重写规则。
- [specification-pattern-demo.cpp](specification-pattern-demo.cpp) — 演示规约模式与 and/or/not 的组合。
  - 属于 [specification-pattern](../cpp-mastery-roadmap/27-stage27/specification-pattern/README.md)
- [state-machine-demo.cpp](state-machine-demo.cpp) — 演示状态机模式：Order 生命周期与显式状态转换。
  - 属于 [state-machine](../cpp-mastery-roadmap/08-stage8/state-machine/README.md)
- [strategy-pattern-demo.cpp](strategy-pattern-demo.cpp) — 演示策略模式：BubbleSort、QuickSort 与 InsertionSort。
  - 属于 [strategy-pattern](../cpp-mastery-roadmap/09-stage9/strategy-pattern/README.md)
- [structured-bindings.cpp](structured-bindings.cpp) — 演示 C++17 结构化绑定在 pair、tuple、数组与结构体上的用法。
- [task-scheduler-error-handling.cpp](task-scheduler-error-handling.cpp) — 演示 TaskScheduler 的错误处理：取消、异常安全与优雅关闭，并用 asserts 验证行为。
  - 属于 [task-scheduler](../cpp-mastery-roadmap/05-stage5/task-scheduler/README.md)
- [template-basics.cpp](template-basics.cpp) — 演示函数模板、类模板与模板特化。
- [template-instantiation.cpp](template-instantiation.cpp) — 演示 C++ 中的模板实例化与两阶段查找。
- [template-method-demo.cpp](template-method-demo.cpp) — 演示模板方法模式：基类定义算法骨架，子类覆写各个步骤。
  - 属于 [template-method](../cpp-mastery-roadmap/15-stage15/template-method/README.md)
- [type-object-demo.cpp](type-object-demo.cpp) — 演示 Type Object 模式：把“类型”表示为数据对象，而不是为每种类型硬编码一个子类（游戏实体）。
  - 属于 [type-object](../cpp-mastery-roadmap/31-stage31/type-object/README.md)
- [undefined-behavior.cpp](undefined-behavior.cpp) — 演示常见的未定义行为场景及其规避方法。
- [unit-of-work-demo.cpp](unit-of-work-demo.cpp) — 演示工作单元模式：跟踪一组对象的所有改动，并作为单个原子事务提交。
  - 属于 [unit-of-work](../cpp-mastery-roadmap/30-stage30/unit-of-work/README.md)
- [value-categories.cpp](value-categories.cpp) — 演示 lvalue、rvalue、prvalue、xvalue 与 glvalue。
- [views-chunk-split.cpp](views-chunk-split.cpp) — 演示 C++23 views::chunk 与 views::split：惰性地批量切分与分词范围，无需手工维护索引。
- [virtual-dispatch-and-slicing.cpp](virtual-dispatch-and-slicing.cpp) — 演示虚函数分发、对象切片与多态。
- [virtual-dispatch-example.cpp](virtual-dispatch-example.cpp) — 演示 C++ 中的虚函数分发、多态与对象模型。
- [visitor-pattern-demo.cpp](visitor-pattern-demo.cpp) — 演示访问者模式：对 Shapes 实现双重分发。
  - 属于 [visitor-pattern](../cpp-mastery-roadmap/12-stage12/visitor-pattern/README.md)
- [web-server-benchmark.cpp](web-server-benchmark.cpp) — 面向 web_server 项目的真实 HTTP 基准测试。
  - 属于 [web-server](../cpp-mastery-roadmap/04-stage4/web-server/README.md)

## 现代 C++ 与标准库

- [cpp20-concepts-complete.cpp](cpp20-concepts-complete.cpp) — 演示 C++20 concepts、requires 子句与受约束模板。
- [cpp20-concepts.cpp](cpp20-concepts.cpp) — 演示用 C++20 concepts 为模板添加约束。
- [cpp20-consteval.cpp](cpp20-consteval.cpp) — 演示 C++20 的 consteval 与 constinit。
- [cpp20-coroutines-complete.cpp](cpp20-coroutines-complete.cpp) — 演示 C++20 协程：生成器与任务。
- [cpp20-coroutines.cpp](cpp20-coroutines.cpp) — 演示 C++20 协程基础。
- [cpp20-format.cpp](cpp20-format.cpp) — 演示用 C++20 std::format 进行字符串格式化。
- [cpp20-ranges.cpp](cpp20-ranges.cpp) — 演示 C++20 ranges 库。
- [cpp20-span.cpp](cpp20-span.cpp) — 演示用 C++20 std::span 提供非拥有型视图。
- [cpp23-features-complete.cpp](cpp23-features-complete.cpp) — 演示本仓库中用到的 C++23 特性。
- [cpp23-ranges-views.cpp](cpp23-ranges-views.cpp) — 演示 C++23 对 `<ranges>` 的新增内容：views::zip、views::enumerate、ranges::to 与 views::as_rvalue。
- [cpp23-small-features.cpp](cpp23-small-features.cpp) — 演示 C++23 中较小的库特性：std::string::contains、std::to_underlying、std::byteswap 与 std::expected 的 value_or。
- [std-any.cpp](std-any.cpp) — 演示用 std::any 实现类型擦除容器。
- [std-apply-invoke.cpp](std-apply-invoke.cpp) — 演示用 std::apply 与 std::invoke 调用泛型可调用对象。
- [std-bit-cpp20.cpp](std-bit-cpp20.cpp) — 演示 C++20 `<bit>` 库：popcount、bit_width 与位操作工具。
- [std-chrono-advanced.cpp](std-chrono-advanced.cpp) — 演示进阶 std::chrono：durations、clocks 与 time points。
- [std-chrono-format.cpp](std-chrono-format.cpp) — 演示用 std::format 格式化 std::chrono 的 durations 与 time points。
- [std-chrono-literals.cpp](std-chrono-literals.cpp) — 演示 C++20 chrono 字面量，例如 100ms 与 2s。
- [std-chrono.cpp](std-chrono.cpp) — 演示 C++17 chrono 库的时间操作。
- [std-clamp.cpp](std-clamp.cpp) — 演示用 std::clamp 把数值限制在某个区间内。
- [std-expected.cpp](std-expected.cpp) — 演示用 std::expected 返回值或错误。
- [std-filesystem-advanced.cpp](std-filesystem-advanced.cpp) — 演示进阶 std::filesystem：递归、权限与符号链接。
- [std-filesystem-cpp20.cpp](std-filesystem-cpp20.cpp) — 演示用 std::filesystem 处理路径、目录与遍历。
- [std-filesystem.cpp](std-filesystem.cpp) — 演示 C++17 filesystem 库。
- [std-flat-map.cpp](std-flat-map.cpp) — 演示 C++23 的 std::flat_map 与 std::flat_set。
- [std-format.cpp](std-format.cpp) — 演示 std::format 的字符串格式化与格式说明符。
- [std-function.cpp](std-function.cpp) — 演示用 std::function 表示函数对象与回调。
- [std-generator.cpp](std-generator.cpp) — 演示 C++23 std::generator：无需手写 promise_type 的协程式惰性序列（可与 cpp20-coroutines-complete.cpp 对照）。
- [std-jthread.cpp](std-jthread.cpp) — 演示 std::jthread：自动 join 与 stop token。
- [std-latch-barrier.cpp](std-latch-barrier.cpp) — 演示用 std::latch 与 std::barrier 协调线程。
- [std-mdspan.cpp](std-mdspan.cpp) — 演示 C++23 std::mdspan 多维视图。
- [std-optional-advanced.cpp](std-optional-advanced.cpp) — 演示进阶 std::optional 用法：链式调用、value_or 与 monadic 调用。
- [std-optional.cpp](std-optional.cpp) — 演示 std::optional 的用法与常见模式。
- [std-print-complete.cpp](std-print-complete.cpp) — 演示 C++23 的 std::print 与 std::println 输出函数。
- [std-print-cpp23.cpp](std-print-cpp23.cpp) — 演示 C++23 的 std::print 与 std::println 输出函数。
- [std-random.cpp](std-random.cpp) — 演示 C++17 随机数生成。
- [std-ranges-advanced.cpp](std-ranges-advanced.cpp) — 演示进阶 std::ranges：views、adaptors 与 algorithms。
- [std-source-location.cpp](std-source-location.cpp) — 演示用 std::source_location 获取编译期源码信息。
- [std-span-complete.cpp](std-span-complete.cpp) — 演示 std::span：针对连续序列的视图。
- [std-string-view-advanced.cpp](std-string-view-advanced.cpp) — 演示进阶 std::string_view：裁剪空白、分割与生命周期规则。
- [std-string-view.cpp](std-string-view.cpp) — 演示用 std::string_view 表示非拥有型字符串引用。
- [std-tuple.cpp](std-tuple.cpp) — 演示用 std::tuple 返回多个值。
- [std-variant-advanced.cpp](std-variant-advanced.cpp) — 演示进阶 std::variant 用法：访问、get_if 与 monadic 调用。
- [std-variant.cpp](std-variant.cpp) — 演示用 std::variant 实现类型安全的联合体。

## 陷阱

- [pitfall-dangling-reference.cpp](pitfall-dangling-reference.cpp) — 演示悬垂引用陷阱及其修复方法。
- [pitfall-iterator-invalidation.cpp](pitfall-iterator-invalidation.cpp) — 演示迭代器失效陷阱及其修复方法。
- [pitfall-object-slicing.cpp](pitfall-object-slicing.cpp) — 演示对象切片陷阱及其修复方法。
