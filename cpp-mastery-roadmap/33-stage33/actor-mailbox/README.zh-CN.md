# Actor 邮箱

有界邮箱 + 单 actor 线程：handler 不持锁运行，生产者不会阻塞 actor。

handler 可以调用自己邮箱的 `stop()`（毒丸消息）；此时不能 join 自己正在运行的线程。

## 学习目标

- 为什么"单消费者线程"让消息处理内部无需加锁
- 两阶段等待：运行 handler 前先释放锁
- 有界邮箱作为背压（满时 send 返回 false）
- 优雅退出：先排空再退出 vs 直接丢弃

## 非目标

- Actor 监督树 / 重启
- 分布式 actor
- 优先级邮箱

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
