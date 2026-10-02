# 生成器流水线（C++23）

基于 `std::generator` 的惰性拉取式流水线：iota / filter / transform /
take / fold，早退出会向上传播到所有上游阶段。

## 学习目标

- 把一个阶段写成协程：整个阶段就是一个生成器
- 阶段间类型演化（transform 让 `int -> std::string`）
- 证明惰性：部分消费前后的运行次数断言
- 与 ranges 视图对比：语义相同、手搓机制

## 非目标

- 并行执行 / 双向通道
- 流水线内错误传播
- ranges adaptor 糖（见 examples/cpp23-ranges-views.cpp）

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
