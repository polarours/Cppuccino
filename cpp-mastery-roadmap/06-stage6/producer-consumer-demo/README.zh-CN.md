# 生产者-消费者演示

本项目使用 Stage 6 ring-buffer 项目里的线程安全 `ring_buffer::RingBuffer`，演示经典的**生产者-消费者模式**。

## 学习目标

- 理解生产者-消费者并发模式
- 学会使用带线程安全 push/pop 的共享缓冲区
- 练习使用条件变量（由 RingBuffer 内部实现）
- 演示缓冲区满时的背压处理

## 项目结构

- `main.cpp`：带多个生产者和消费者的演示程序
- `tests/pcon_tests.cpp`：线程安全环形缓冲区用法的单元测试
- `ring_buffer.hpp`：从已有的 ring-buffer 项目引入

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/producer_consumer_demo
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 相关项目

- [ring-buffer](../ring-buffer/)：线程安全的循环缓冲区实现
- [connection-pool](../connection-pool/)：另一个使用共享资源的并发模式
- [guarded-suspension](../../../cpp-mastery-roadmap/32-stage32/guarded-suspension)：等待条件满足后再继续

## 建议的扩展

- 添加 shutdown 信号以优雅地停止消费者
- 实现带背压反馈的有界生产者
- 添加统计跟踪（吞吐量、延迟）
