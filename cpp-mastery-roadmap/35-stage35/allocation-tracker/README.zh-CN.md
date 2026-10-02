# 分配追踪器

进程级分配计数（活跃/峰值/失败）+ 供 STL 容器使用的无状态计数分配器，
以及 new_handler 钩子。

## 学习目标

- 原子计数器：统计场景 relaxed 序足够
- 用 compare_exchange 循环跟踪峰值
- STL 分配器要求：无状态分配器的 rebind 相等性
- new_handler 契约：抛 bad_alloc 或释放内存后重试

## 非目标

- 替换全局 operator new（字节级泄漏检测交给 ASan）
- 采样 / 火焰图
- 跨线程归属统计

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
