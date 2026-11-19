# SIMD 模式

手动 SIMD 风格向量化数学运算。

逐元素操作按同一下标读两个向量，较短的一方决定结果长度，
不会读越过它的末尾。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
