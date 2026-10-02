# Base64 编解码

RFC 4648 base64 编解码，严格校验（`=` 位置、字母表），提供不抛异常的
`isValid()`。

## 学习目标

- 位分组：3 字节 -> 4 个六位组，余数 1/2 的填充规则
- 编码用索引表，解码用范围检查
- 为什么长度必须是 4 的倍数、`=` 的合法位置
- 全 256 字节值往返作为测试判据

## 非目标

- Base64url（URL 安全字母表）
- 流式分块编码
- MIME 76 字符换行

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
