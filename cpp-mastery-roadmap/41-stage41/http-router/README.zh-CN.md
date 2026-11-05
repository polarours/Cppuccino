# HTTP 路由器

按方法 + 路径匹配路由，支持 `:param` 段，参数提取为 map 交给 handler。

## 学习目标

- 按段数与字面量/参数规则做模式匹配
- 无匹配时返回 404 的分发逻辑
- 不用正则提取命名路径参数

## 非目标

- 中间件管道
- 查询串与请求头
- 生产环境路由边界情况

## 线程安全

路由表在同一把锁下读写，所以可以在别的线程匹配请求的同时添加路由。
handler 本身是在释放锁之后调用的：若 handler 想再注册一条路由，就会
死锁在一把它已经持有的锁上。

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
