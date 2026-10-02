# 跳表

概率型有序集合：带随机层级的有序单链表，期望 O(log n) 查找/插入。

## 学习目标

- 前向指针塔与多层遍历
- p=1/2 的 randomLevel()，测试用确定性随机种子
- 三路比较实现集合语义（`!(a<b) && !(b<a)`）
- 为什么跳表比二叉树更适合并发实现

## 非目标

- 删除
- 线程安全
- 最坏情况平衡保证（那是树的领域）

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
