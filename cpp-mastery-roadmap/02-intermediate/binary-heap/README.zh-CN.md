# 二叉堆

数组实现的二叉堆，自定义序（默认最大堆），push/pop 为 O(log n)，
自底向上 heapify 为 O(n)。

## 学习目标

- vector 上的父子下标计算
- siftUp 与 siftDown 的职责划分
- 为什么从 n/2 开始 heapify 是 O(n) 而非 O(n log n)
- 比较器模板参数（std::less = 最大堆）

## 非目标

- decrease-key / 任意位置删除
- d 叉堆或斐波那契堆
- 线程安全

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
