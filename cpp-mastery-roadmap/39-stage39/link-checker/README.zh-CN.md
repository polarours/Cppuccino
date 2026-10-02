# 链接检查器

校验 markdown 中的相对链接是否指向已知存在的文件，支持路径归一化与可选的锚点校验。

## 学习目标

- 不接触文件系统地做词法路径归一化（`./`、`../`）
- 提取时跳过行内代码与外部链接
- 锚点跟踪：仅当调用方跟踪锚点时才报告缺失的 `#anchor`
- 把常见的文档杂活变成可测试的单元

## 非目标

- 抓取 HTTP 链接
- 遍历真实文件系统（由调用方提供文件集合）
- 完整 markdown 解析

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/link_checker_example
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```
