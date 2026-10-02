# Markdown 解析器

一个最小的 markdown 分块解析器：标题、列表项、引用、围栏代码块、行内链接与行内格式。

## 学习目标

- 用状态机解析逐行文本格式（代码围栏状态）
- 代码围栏内原文保留，避免内部语法被二次解析
- 用小型扫描器提取行内链接
- 去除行内标记符号而不破坏周围文本

## 非目标

- 完整 CommonMark 兼容
- 输出 HTML
- 嵌套块结构

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/markdown_parser_example
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```
