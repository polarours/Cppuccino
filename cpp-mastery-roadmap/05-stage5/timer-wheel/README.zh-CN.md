# 时间轮

单层时间轮：按绝对 tick O(1) 调度/取消，轮转时重新检查到期时间，
远期定时器安全跨越整圈。

## 学习目标

- 槽位下标 = 绝对 tick % 轮盘大小
- 为什么存绝对到期时间能避免轮转后的重复触发
- 按 id 取消（扫描一轮）
- 与二叉堆定时器队列对比（O(log n)、无槽位粒度）

## 非目标

- 分层时间轮（min/max timing wheel）
- 线程安全（生产环境需加互斥锁）
- 高精度实时保证

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
