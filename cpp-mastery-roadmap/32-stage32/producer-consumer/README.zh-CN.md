# 生产者消费者

线程安全的有界缓冲区。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
