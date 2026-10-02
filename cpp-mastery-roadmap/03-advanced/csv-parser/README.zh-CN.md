# CSV 解析器

RFC-4180 风格 CSV：引号字段支持逗号/换行/双写引号，容忍 CRLF，首行表头，
format() 保证无损往返。

## 学习目标

- 引号规则的状态机（引号内 vs 引号外）
- 双写引号（""）为什么存在及如何反转义
- 短行的行-列对齐
- format/parse 往返作为正确性判据

## 非目标

- schema 校验 / 类型推断
- 从文件流式读取（输入是字符串）
- 逗号以外的分隔符

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
