# Slab 分配器

按大小分级的内存缓存。

## 块的大小

块的大小是把请求**向上取整到 2 的幂**，且至少 16 字节。比 slab 还大的块会
单独占一个 slab，这样指针后面的内存就真的和块一样大。

完全给不出来的请求会抛 `std::bad_alloc`，而不是永远地往上取整。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
