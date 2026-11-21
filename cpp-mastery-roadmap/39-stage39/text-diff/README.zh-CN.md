# 文本 Diff（LCS）

基于最长公共子序列的经典逐行 diff，支持渲染与相似度评分。

相似度 = 相等行数 / 两份文件中较多的一份的行数，不是 hunk 数；
一次删除 + 一次插入不能把分母算成两行。

空行也是一行：切分文档时会把空行保留下来，这样被比较的两份文档才能按同样的
行号对齐。

## 学习目标

- LCS 动态规划（自底向上回溯出编辑脚本）
- 编辑脚本语义：Equal / Insert / Delete 的顺序
- 从 diff 推导相似度度量
- unified-diff 风格渲染

## 非目标

- Myers O(ND) 算法
- 词级或字符级 diff
- 带上下文行的 hunk 分组

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/text_diff_example
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```
