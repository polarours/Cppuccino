# Result 类型

无异常的类型安全错误处理。

`parseInt` 只接受完整的整数。标准库的数字函数读到第一个读不懂的字符就停下，
所以没有这道检查的话，"12abc" 会被读成 12，后面的字符**一声不吭**地被丢掉。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
