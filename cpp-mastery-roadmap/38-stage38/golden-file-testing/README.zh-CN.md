# 黄金文件测试

快照式断言工具：失配时逐行 diff、`UPDATE_GOLDEN` 显式接受运行、
针对易变输出（时间戳、id）的归一化规则。

## 学习目标

- 什么时候快照测试优于示例断言（CLI 输出、序列化文档）
- 为什么"缺失黄金文件 + UPDATE_GOLDEN=1"必须显式（不许静默接受）
- 归一化解决非确定字段
- 有界 diff 报告保证失败信息可读

## 非目标

- 二进制/图片比对
- 审批式测试 UI 工具
- CI 自动更新黄金文件

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
