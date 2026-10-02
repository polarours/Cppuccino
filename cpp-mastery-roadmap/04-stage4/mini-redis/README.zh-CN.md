# Mini Redis

内存 KV 孭储，实现类 Redis 命令集（SET/GET/DEL/INCR/EXPIRE/TTL/DBSIZE），
RESP 风格应答，惰性 TTL 过期。

## 学习目标

- 命令分发表：参数个数检查与统一错误应答
- 惰性过期：为什么访问时校验即可保证正确
- TTL 语义：-2/-1/进位、写路径上的过期
- INCR 的整数解析纪律（拒绝尾部非数字）

## 非目标

- RESP multibulk 解析与带引号字符串
- 真实 TCP 监听（把 Store 接进 stage 4 的服务器即可）
- 持久化 / 复制

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
