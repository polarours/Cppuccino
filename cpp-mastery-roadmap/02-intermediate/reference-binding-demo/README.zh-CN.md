# 引用绑定演示

本项目演示 C++ 引用绑定规则——理解移动语义、转发引用和现代 C++ API 设计的基础概念。

## 学习目标

- 理解左值引用绑定规则
- 理解右值引用绑定规则
- 学习函数参数如何绑定到引用
- 理解引用折叠与完美转发
- 理解 `const` 左值引用的生命周期延长

## 演示的概念

1. **左值引用绑定**：`int&` 只能绑定到左值
2. **const 左值引用绑定**：`const int&` 可绑定左值和右值（生命周期延长）
3. **右值引用绑定**：`int&&` 只能绑定到右值（或被显式 move 的左值）
4. **函数参数绑定**：引用参数与调用方参数的交互
5. **引用折叠**：模板中 `T&&` 的规则（完美转发）
6. **生命周期延长**：绑定到 `const&` 的临时对象活得更久

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/reference_binding_demo
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 相关文档

- [左值引用和右值引用](../../../docs/中文版/左值引用和右值引用.md)
- [std::move 的语义](../../../docs/中文版/深入理解 move 语义.md)
- [移动语义示例](../../../examples/move-semantics-example.cpp)
