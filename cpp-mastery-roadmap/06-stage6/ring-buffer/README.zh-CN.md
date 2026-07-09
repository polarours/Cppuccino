# 环形缓冲区

一个线程安全的环形缓冲区实现。

## 学习目标

- 理解环形缓冲区数据结构
- 练习生产者-消费者模式的线程同步
- 学习无锁和有锁实现
- 实现阻塞和非阻塞操作

## 非目标

- 构建生产级环形缓冲区
- 实现无锁算法
- 支持多生产者/消费者

## 功能

- 线程安全的 push/pop 操作
- 带超时的阻塞 pop
- 无需移除的 peek
- FIFO 顺序
- 可配置容量

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/ring_buffer_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "ring_buffer.hpp"

ring_buffer::RingBuffer<int> buffer(10);

// 生产者
buffer.push(42);

// 消费者
auto item = buffer.pop();
if (item) {
    std::cout << "Got: " << *item << "\n";
}

// 阻塞 pop
auto item2 = buffer.popWait(std::chrono::seconds(1));
```

## 建议的下一步

- 实现无锁版本
- 添加多生产者/消费者支持
- 添加优先级支持
- 实现批量操作
