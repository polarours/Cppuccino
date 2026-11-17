# 参数化测试

数据驱动的测试执行。

测试里那几个数字不会溢出：`isPrime()` 的循环问的是除法而不是乘法，
`fibonacci()` 用 `long long` 计数 —— 第 47 个斐波那契数装不进 `int`。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
