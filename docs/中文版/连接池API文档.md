# 连接池 API

## 1. 是什么

`ConnectionPool<T>` 复用工厂创建的值，并限制同时借出的连接数量。

> 分开理解空闲连接与已借出连接的计数。

## 2. 与逐次创建的对比

| 方式 | 优点 | 调用方责任 |
|---|---|---|
| 每次创建 | 所有权简单 | 承担重复初始化成本 |
| 连接池 | 复用并限制容量 | 每次借出恰好归还一次 |

## 3. 核心概念与 API

- `Creator = std::function<T()>` 创建连接；`Deleter` 只是未使用的类型别名，并没有注册清理回调。
- 构造参数为工厂、初始数量、最大数量、等待超时（默认 5000 ms）。调用方必须保证 `0 <= minSize <= maxSize`、`maxSize > 0`、工厂可调用且超时非负；当前实现不验证这些参数。
- `acquire()` 返回空闲或新建连接，否则等待归还。池已关闭或等待超时会抛出 `std::runtime_error`。
- `release(T)` 归还连接；`size()` 返回空闲数量；池开启期间 `activeConnections()` 返回已借出数量。
- `close()` 拒绝后续获取并唤醒等待者，不负责 join 调用线程，也不立即销毁空闲队列。

## 4. 计数问题与解决方案

仅在创建时递增、每次归还却递减，会造成无符号计数下溢和容量超限。每条成功获取路径（包括复用、等待唤醒）都要递增；工厂成功后才能计入借出数量。

池开启、参数合法且借还配对时，保持 `空闲 + 借出 <= maxSize`。只有空闲为零才创建，因此此时比较借出数量与最大容量即可。

## 5. 可运行示例

```cpp
#include "connection_pool.hpp"
#include <cassert>
#include <chrono>
#include <string>

int main() {
    int counter = 0;
    connection_pool::ConnectionPool<int> pool(
        [&]() { return ++counter; }, 1, 1, std::chrono::milliseconds(10));
    auto conn = pool.acquire();
    assert(pool.activeConnections() == 1);
    bool timedOut = false;
    try {
        pool.acquire();
    } catch (const std::runtime_error& error) {
        timedOut = std::string(error.what()) == "Connection pool timeout";
    }
    assert(timedOut);
    pool.release(conn);
    assert(pool.activeConnections() == 0);
    assert(pool.size() == 1);
    pool.close();
}
```

把代码块保存为 `/tmp/pool-api.cpp`，从仓库根目录执行：

```bash
g++ -std=c++17 -pthread -Wall -Wextra -Werror -Icpp-mastery-roadmap/06-stage6/connection-pool/include /tmp/pool-api.cpp -o /tmp/pool-api
/tmp/pool-api
```

## 6. 并发、异常与限制

- 队列访问、工厂调用、关闭操作共用一把互斥锁。工厂不能重入该池；慢工厂会阻塞其他操作。等待超时不覆盖获取互斥锁及执行工厂的时间。
- 工厂异常原样传播且不消耗容量。`T` 的移动若抛异常，不提供强异常安全保证；优先使用可无异常移动的 RAII 类型。
- 每次借出必须恰好归还一次；不检测外来连接与重复归还。裸指针不会自动 delete，应使用 RAII 管理资源。
- 关闭后 `release()` 丢弃传入值但不更新借出计数，因此关闭后的计数不能视为实时借出量。空闲值在池析构时销毁。
- `close()` 唤醒等待者不等于支持并发析构。必须先 join 所有调用线程再销毁池。C++ 没有 `finally`，应使用作用域守卫或显式异常安全归还。
- 单次查询是同步的，但分别读取 `size()` 和 `activeConnections()` 不是原子快照。不保证等待公平性。

## 7. 总结

正确计数才能保证容量约束；有互斥锁不代表资源所有权管理正确。

[项目源码](../../cpp-mastery-roadmap/06-stage6/connection-pool/) · [并发示例](../../examples/connection-pool-concurrency.cpp)
