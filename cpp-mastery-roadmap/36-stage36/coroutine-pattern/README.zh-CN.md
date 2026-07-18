# 协程模式

C++20 协程，用于惰性求值和生成器。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
