# 仓库检查脚本

这些脚本负责保证仓库不出问题。它们在每次 push 时于 CI 中运行，而且跑的
就是贡献者在本地跑的同一批脚本 —— 所以本地跑绿了，CI 就一定跑绿。

```bash
tools/run_all_checks.sh          # 文档类检查
tools/run_all_checks.sh --full   # 外加全部项目的构建与测试
```

## 检查项

| 脚本 | 能查出什么 |
|------|------------|
| `check_docs.py` | `docs/en/` 里文件名是英文、正文却是中文的文件；中英两半互相漂移 |
| `check_links.py` | 失效的相对链接或 `#锚点` |
| `check_doc_examples.py` | 文档里已编不过或跑不通的完整示例 |
| `check_examples.py` | `examples/` 下编不过的独立示例，头注释已经描述不准构建方式的示例，或两个示例写同一个可执行文件 |
| `check_stats.py` | README 里已经和仓库实际对不上的统计数字 |
| `check_project_readmes.py` | 项目 README 与它的中英另一半脱节，或写了项目并不构建的可执行文件 |
| `check_overview_parity.py` | 中英两份路线图索引列出的阶段或项目不一致 |
| `check_docs.py` | 代码块被截断，导致花括号不配平 |
| `build_all_projects.py` | 配置、构建或测试失败的项目 |

## 语言判定是怎么做的

判断"这篇文档是不是中文"最直白的办法是数 CJK 字符和拉丁字符的比例。
但在本仓库里这行不通。API 参考文档大半是表格行：

```markdown
| `save` | `bool save(const std::string& filename) const` | 保存到文件 |
```

标识符是拉丁的，描述是中文，比例算下来"大部分是英文"，可正文明明是中文。

所以判定改成逐行进行：一行满足"CJK 字符至少 2 个"且"CJK 字符数不少于拉丁
**单词**数"时，判为中文占优；完全没有 CJK 的行不提供语言信号，两边都不计。
计数前会先去掉围栏代码块，因此写错语言的注释不会影响判定结果。

## 两半是怎么配对的

`docs/en/` 与 `docs/中文版/` 没有共同的命名约定，所以一份文档的两半靠结构
配对：按顺序的代码块列表，逐个词法单元比较。字符串字面量的内容会被清空，
因为翻译后的文档用自己语言打印消息，跑的却是同一个程序。

陷阱文章则改为人工配对，列在 `pitfall_pairs.tsv` 里。它们的中文文件名是
翻译（`悬垂指针` 对 `dangling-pointer`），没有可匹配的拼写；而结构回退在
两篇文章恰好用了相似示例时又会配错。

配对规则由
[bilingual-doc-checker](../cpp-mastery-roadmap/39-stage39/bilingual-doc-checker/)
这个 C++ 库实现。

## 编译器

检查脚本默认用 `g++`，并遵循 `CXX` 环境变量，所以 CI 可以指定更新的工具链：

```bash
CXX=g++-14 tools/check_examples.py
```

如果示例需要的标准编译器不认识，会报告为"跳过"而不是"失败"，
这样用较旧编译器的 runner 不会因此挂掉。
