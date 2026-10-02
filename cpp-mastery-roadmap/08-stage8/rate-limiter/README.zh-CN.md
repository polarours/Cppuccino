# 限流器

一个头文件下三种经典算法：令牌桶（突发 + 持续速率）、固定窗口计数
（按 key）、滑动窗口日志。

## 学习目标

- 令牌桶：突发容量 vs 补充速率、补充到容量封顶
- 固定窗口：便宜但允许边界双倍突发
- 滑动窗口日志：精确但要存每次请求时间戳
- 把 `now` 作为参数注入，让时间逻辑在测试中可确定复现

## 非目标

- 分布式限流（Redis / Lua 脚本）
- 加权请求（按成本准入）
- 响应头（X-RateLimit-*）

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
