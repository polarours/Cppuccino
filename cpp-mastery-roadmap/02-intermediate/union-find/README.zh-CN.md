# 并查集（DSU）

不相交集合并：迭代式路径压缩 + 按秩合并。

## 学习目标

- find 全路径压缩：均摊近似常数时间
- 为什么按秩合并能保持树扁平
- 组件计数：只在真正合并时递减
- 迭代压缩避免长链上的深递归栈风险

## 非目标

- 除秩以外的加权合并
- 持久化 / 可回滚（rollback DSU）
- 并发访问

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
