# 静态初始化顺序演示

本项目演示 C++ 中的**静态初始化顺序难题（static initialization order fiasco）**，并展示使用函数作用域静态变量的解决方案。

## 学习目标

- 理解静态初始化顺序问题（即"静态初始化顺序难题"）
- 了解为什么跨编译单元的 global/static 变量初始化顺序是未指定的
- 学习使用函数作用域静态变量的解决方案（Meyers 单例）

## 项目结构

- `src/bad_demo.cpp`：演示初始化顺序问题的全局计数器对象
- `src/main.cpp`：主程序，对比展示坏做法和好做法
- `src/counter.cpp`：带共享静态状态的 Counter 类实现
- `include/counter.hpp`：Counter 类头文件
- `tests/static_init_tests.cpp`：单元测试

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/static_init_demo
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 问题所在

在 C++ 中，不同编译单元之间非局部静态变量的初始化顺序是未指定的。如果一个全局对象在使用另一个全局对象时后者尚未构造，就会导致未定义行为。

## 解决方案

使用函数作用域静态变量（Meyers 单例）可以保证首次使用时才初始化，从而完全避免静态初始化顺序问题。
