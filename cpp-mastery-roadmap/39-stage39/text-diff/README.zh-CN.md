# 文本 Diff（LCS）

基于最长公共子序列的经典逐行 diff，支持渲染与相似度评分。

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
