# Connection Pool API 文档

## 概述

`connection-pool` 是一个通用的连接池模板类，支持最小/最大连接数限制、超时获取和线程安全访问。

## 核心类型

### Creator / Deleter

```cpp
using Creator = std::function<T()>;
using Deleter = std::function<void(T*)>;
```

- `Creator`：工厂函数，用于创建新连接
- `Deleter`：析构函数，用于销毁连接（当前实现中未使用，可通过自定义释放逻辑扩展）

## ConnectionPool<T> 类 API

### 构造

| 方法 | 签名 | 说明 |
|---|---|---|
| 构造器 | `ConnectionPool(Creator creator, std::size_t minSize, std::size_t maxSize, std::chrono::milliseconds timeout = 5000ms)` | 创建池，预热 minSize 个连接 |

### 连接管理

| 方法 | 签名 | 说明 |
|---|---|---|
| `acquire` | `T acquire()` | 从池中获取连接，超时抛出异常 |
| `release` | `void release(T connection)` | 归还连接到池中 |
| `close` | `void close()` | 关闭池，唤醒所有等待线程 |

### 状态查询

| 方法 | 签名 | 说明 |
|---|---|---|
| `size` | `std::size_t size() const` | 返回池中可用连接数 |
| `activeConnections` | `std::size_t activeConnections() const` | 返回当前活跃连接数 |

### 线程安全

- `acquire` / `release` 通过 `std::mutex` 保护共享状态
- `condition_variable` 用于等待空闲连接
- `activeConnections_` 和 `closed_` 是 `std::atomic`，无锁读取

## 使用示例

```cpp
#include "connection_pool.hpp"
#include <iostream>
#include <thread>

struct DBConnection {
    int id;
    bool connected = true;
};

int main() {
    connection_pool::ConnectionPool<DBConnection> pool(
        [](){ return DBConnection{.id = ++counter}; },
        2,   // 最小连接数
        5,   // 最大连接数
        std::chrono::milliseconds(1000)
    );

    std::cout << "Pool size: " << pool.size() << "\n";

    // 并发获取连接
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            auto conn = pool.acquire();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            pool.release(std::move(conn));
        });
    }
    for (auto& t : threads) t.join();

    pool.close();
    return 0;
}
```

## 异常安全

| 场景 | 行为 |
|---|---|
| 池已关闭 | `acquire()` 抛出 `std::runtime_error("Pool is closed")` |
| 超时未获取 | `acquire()` 抛出 `std::runtime_error("Connection pool timeout")` |
| `close()` 调用 | 唤醒所有等待线程，后续 `acquire()` 失败 |

## 最佳实践

1. **最小连接数**：设置为预期并发量的 10-20%，避免频繁创建
2. **超时设置**：根据业务容忍度设置，一般 1-5 秒
3. **资源释放**：确保 `release()` 在 finally 块中调用，防止连接泄漏
4. **优雅关闭**：先设置 `closed_`，再 `notify_all()`，最后等待线程退出