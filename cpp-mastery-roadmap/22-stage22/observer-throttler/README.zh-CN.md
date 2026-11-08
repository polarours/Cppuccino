# 观察者节流器

一个带节流事件分发的观察者。

## 线程安全

这个分发器内部没有锁。它是按"一个线程发事件、另一个线程收事件"来设计的：
如果好几个线程同时往里发事件，放行的次数就会超过设定的间隔；而在分发
进行当中换掉 handler，等于在另一个线程正调用它的时候改那个
`std::function`。

要么自己套一把锁，要么一个线程用一个分发器。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
