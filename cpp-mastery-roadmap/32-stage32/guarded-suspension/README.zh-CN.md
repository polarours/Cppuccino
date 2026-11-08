# 保护性暂停

在线程安全访问前等待条件满足。

## 线程安全

对象的状态是原子的，读它不需要锁。正因如此，`waitFor()` 的谓词才可以问对象
自己：

```cpp
#include "guarded_suspension.hpp"

guarded_suspension::GuardedObject obj;

obj.waitFor([&obj]() { return obj.isReady(); });
```

条件变量是在**持有锁**的状态下调用谓词的，所以谓词要是也需要这把锁，就等于
在等一把当前线程已经持有的锁。mutex 是给写者和条件变量自己用的。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
