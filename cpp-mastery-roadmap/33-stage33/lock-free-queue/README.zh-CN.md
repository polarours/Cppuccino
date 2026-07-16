# 无锁队列

使用原子操作的无等待队列。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
