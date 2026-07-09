# 线程池

一个简单的现代 C++ 线程池实现。

## 学习目标

- 理解使用互斥锁和条件变量进行线程同步
- 练习移动独占类型和 std::future/std::packaged_task
- 学习生产者-消费者模式
- 将 RAII 应用于线程管理

## 非目标

- 构建生产级线程池
- 实现工作窃取或任务优先级
- 支持取消或异常传播

## 功能

- 固定大小线程池
- 使用 `enqueue()` 提交任务并获取 `std::future`
- `waitForAll()` 等待所有任务完成
- `shutdown()` 优雅关闭
- 线程安全设计

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/thread_pool_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "thread_pool.hpp"

thread_pool::ThreadPool pool(4);

// 提交任务
auto future = pool.enqueue([]() {
    return 42;
});

// 获取结果
int result = future.get();  // 42

// 带参数的任务
auto sum = pool.enqueue([](int a, int b) {
    return a + b;
}, 3, 4);

// 等待所有任务完成
pool.waitForAll();
```

## 建议的下一步

- 添加任务取消支持
- 实现工作窃取以提高负载均衡
- 添加任务优先级
- 添加动态线程池调整
