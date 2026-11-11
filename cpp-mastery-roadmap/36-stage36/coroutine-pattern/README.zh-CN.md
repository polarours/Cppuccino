# 协程模式

C++20 协程，用于惰性求值和生成器。

生成器可以移交给另一个变量，也可以覆盖掉一个已经握着生成器的变量：原来那个
协程会被销毁，不会被留在那里。被移走的生成器什么也没有了，它被问起值的时候会
说出来。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
