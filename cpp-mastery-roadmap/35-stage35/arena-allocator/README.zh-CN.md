# Arena 分配器

用于快速顺序分配的碰撞分配器。

放不下的请求返回 `nullptr`。容量检查用减法而不是加法，巨大的 `size`
不会回绕绕过检查。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
