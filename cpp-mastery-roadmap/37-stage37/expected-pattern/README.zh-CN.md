# Expected 模式

带单子操作的类型安全值或错误。

`divide()` 会拒绝唯一那个结果装不进 `int` 的除法：处理器在上面会陷入陷阱，
程序会死掉，而不是报出一个问题。

`map()` 和 `andThen()` 是**按左值调用**来算出结果类型的，因为它们实际做的
就是那次调用。所以一个对左值和对右值各有一个重载的函数，两处挑的是同一个
重载。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
