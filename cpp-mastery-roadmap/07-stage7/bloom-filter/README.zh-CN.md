# 布隆过滤器

概率型成员判定结构：无假阴性、可调假阳性，双哈希派生 k 个位置、位压缩存储。

位数量为 0 时过滤器什么都不存：`add()` 不做任何事，`contains()` 永远返回 false
（和假阳性率 1 一致）。

## 学习目标

- 为什么布隆过滤器没有假阴性（位只置位、不清除）
- 双哈希：用 2 个基础哈希派生 k 个位置
- 假阳性率公式与 m/k/n 的权衡
- 压缩位存储（uint8_t 数组 + 移位掩码）

## 非目标

- 删除（Counting Bloom Filter）
- 可扩展 / 分区变体
- 密码学哈希要求

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
