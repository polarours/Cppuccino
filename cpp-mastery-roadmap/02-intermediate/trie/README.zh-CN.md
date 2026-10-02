# Trie（前缀树）

小写字母前缀树：词计数、前缀计数、按字典序枚举前缀下的全部单词。

## 学习目标

- 节点结构：children[26] + endsHere + passes 计数器
- 为什么前缀查询是 O(前缀长度)
- 枚举子树下的所有单词（按字符序 DFS）
- 重复处理：去重计数 vs 出现次数

## 非目标

- Unicode / 大写键
- Patricia / 基数压缩
- 删除条目

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
