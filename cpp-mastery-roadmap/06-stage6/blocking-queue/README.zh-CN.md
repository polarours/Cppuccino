# 阻塞队列

有界 MPMC 队列：两个条件变量（非空/非满）、带超时的 push/pop，
close() 唤醒全部等待者。

## 学习目标

- 为什么有界队列一把锁需要两个条件变量
- 基谓词的 wait_for 避免虚假唤醒
- close() 语义：排空后返回 nullopt、push 失败
- 生产者/消费者测试中证明"每项恰好消费一次"

## 非目标

- 无锁实现（见 stage 33 lock-free-queue）
- 优先级排序
- 按消费者公平调度

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
