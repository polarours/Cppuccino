# LFU 缓存

最近最少使用频率缓存，O(1) get/put，频率桶 + LRU 打破平局。

## 学习目标

- 频率桶设计：map<频率, LRU 链表> + 节点迭代器索引
- 为什么频率桶能保持 O(1)
- 频率相同时的平局策略
- 与 LRU 对比（见 ../lru-cache）

## 非目标

- TinyLFU / window 准入
- 线程安全
- TTL 过期

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
