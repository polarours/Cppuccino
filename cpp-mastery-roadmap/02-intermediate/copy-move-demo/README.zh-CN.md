# 拷贝与移动语义演示

一个演示项目，会打印每一次拷贝/移动操作，让你**看到**编译器在每种情况下选择哪个构造函数/赋值运算符。

## 学习目标

- 理解拷贝与移动构造/赋值的区别
- 弄清编译器在何时调用"大五"（构造/拷贝构造/移动构造/拷贝赋值/移动赋值/析构）中的每一个
- 观察生命周期语义：移动之后发生了什么，`const&` 如何延长临时对象的生命周期

## 演示的模式

- **拷贝对象**：从左值拷贝构造、拷贝赋值
- **移动对象**：通过 `std::move` 移动构造、移动赋值
- **按值传递**：临时对象的构造，随后 move（或被拷贝省略）
- **按引用传递**：左值绑定，以及 `const&` 的生命周期延长

每个操作都会打印 `[CONSTRUCT] / [COPY CTOR] / [MOVE CTOR] / [COPY ASSGN] / [MOVE ASSGN] / [DESTRUCT]` 并带对象标签，追踪结果不言自明。

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/copy_move_demo
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 相关文档

- [左值引用和右值引用](../../../docs/中文版/左值引用和右值引用.md)
- [移动语义示例](../../../examples/move-semantics-example.cpp)
