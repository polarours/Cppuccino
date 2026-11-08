# 内存池

预分配固定大小块的内存池。

## 块的大小

零字节的块不算块：这样的池在构造时抛 `std::invalid_argument`。大小装不下
内存的池抛 `std::bad_alloc`。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
