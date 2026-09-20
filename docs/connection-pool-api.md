# Connection Pool API

## 1. What it is

`ConnectionPool<T>` reuses values produced by a factory and limits simultaneous loans.

> Track borrowed connections separately from idle connections.

## 2. Pool versus per-operation creation

| Approach | Benefit | Responsibility |
|---|---|---|
| Create per operation | Simple ownership | Repeated setup cost |
| Pool | Reuse and bounded capacity | Return each loan exactly once |

## 3. Core concepts and API

- `Creator = std::function<T()>` constructs a connection; `Deleter` is an unused alias, not an installed cleanup callback.
- Constructor parameters are creator, initial size, maximum size and wait timeout (default 5000 ms). Supply `0 <= minSize <= maxSize`, `maxSize > 0`, a callable factory and a nonnegative timeout; the implementation does not validate these arguments.
- `acquire()` returns an idle or newly created value, or waits for a return. Closed pools and exhausted waits throw `std::runtime_error`.
- `release(T)` returns a loan; `size()` reports idle values and `activeConnections()` reports borrowed values while the pool is open.
- `close()` rejects future acquisitions and wakes waiters. It does not join callers or destroy the idle queue immediately.

## 4. Accounting problem and solution

Incrementing only on creation but decrementing every return underflows the unsigned count and permits capacity overflow. Increment after every successful acquisition, including reuse and wake-up; increment only after the factory succeeds.

While open, with valid arguments and correctly paired loans, `idle + borrowed <= maxSize`. Creation happens only when idle is zero, so comparing borrowed against the maximum is sufficient.

## 5. Runnable example

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

From the repository root, after saving the block as `/tmp/pool-api.cpp`:

```bash
g++ -std=c++17 -pthread -Wall -Wextra -Werror -Icpp-mastery-roadmap/06-stage6/connection-pool/include /tmp/pool-api.cpp -o /tmp/pool-api
/tmp/pool-api
```

## 6. Concurrency, exceptions and limitations

- Queue access, factory calls and close transitions use the same mutex. A factory must not re-enter the pool; a slow factory blocks other operations. The wait timeout does not bound mutex acquisition or factory execution.
- A factory exception propagates without consuming capacity. Throwing moves of `T` do not have a strong exception-safety guarantee; prefer nonthrowing-movable RAII values.
- Return each acquired value exactly once. Foreign values and duplicate returns are not detected. Raw pointers are not automatically deleted; use RAII ownership.
- `release()` after close discards its argument without updating the borrowed counter, so that counter is not a live-loan metric after close. Idle values are destroyed with the pool.
- `close()` wakes waiters but cannot make destruction concurrent with callers safe. Join all callers before destroying the pool. C++ has no `finally` block: use a scope guard or explicit exception-safe return.
- Individual queries are synchronized, but separate calls to `size()` and `activeConnections()` are not an atomic snapshot. No fairness guarantee is provided.

## 7. Summary

Correct accounting makes the capacity limit meaningful; mutexes alone do not establish correct resource ownership.

[Project source](../cpp-mastery-roadmap/06-stage6/connection-pool/) · [Concurrent example](../examples/connection-pool-concurrency.cpp)
