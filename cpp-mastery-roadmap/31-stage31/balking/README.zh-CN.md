# 省略模式

前置条件不满足时跳过操作。

`isOpen()`/`isSaved()` 读原子状态，转换在互斥锁下进行时，
另一个线程也可以并发询问。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
