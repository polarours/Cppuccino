# 观察者总线

一个事件总线，用于发布-订阅通信。

派发时遍历订阅列表的副本，因此 handler 可以在派发过程中订阅同一个事件；
新订阅的 handler 从下一次 publish 开始生效。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
