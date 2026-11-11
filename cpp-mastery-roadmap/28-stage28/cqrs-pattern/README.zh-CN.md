# CQRS 模式

命令查询职责分离 — 将读写分离。

payload 里的数字就是数字，不会是别的。标准库的数字函数读到第一个读不懂的字符
就停下，所以没有这道检查的话，"1abc" 会被读成 1，后面的字符**一声不吭**地被
丢掉。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
