# 模板引擎

最小 mustache 风格引擎：`{{var}}` 变量、真值段 `{{#k}}...{{/k}}`、反向段
`{{^k}}...{{/k}}`，列表按项渲染。

## 学习目标

- 分词 `{{tag}}` 同时保留字面文本
- 配对闭合标签的递归段渲染
- 上下文链：子项键覆盖父上下文
- bool 与空列表的假值语义

## 非目标

- HTML 转义
- 部分模板 / 继承
- 空白控制

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
