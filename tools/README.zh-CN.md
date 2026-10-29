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
| `check_docs.py` | `docs/en/` 里文件名是英文、正文却是中文的文件；中英两半互相漂移；代码块被截断，导致花括号不配平 |
| `check_links.py` | 失效的相对链接或 `#锚点` |
| `check_doc_examples.py` | 文档里已编不过、跑不通，或者读了已释放内存的完整示例 |
| `check_examples.py` | `examples/` 下编不过的独立示例，头注释已经描述不准构建方式的示例，两个示例写同一个可执行文件，正文还是中文的示例，或泄漏的示例 |
| `check_stats.py` | README 里已经和仓库实际对不上的统计数字 |
| `check_project_readmes.py` | 项目 README 与它的中英另一半脱节，或写了项目并不构建的可执行文件，或语言用错了 |
| `check_readme_examples.py` | 项目 README 里的 C++ 示例已经编不过 |
| `check_warnings.py` | 编译器在项目或示例里发现的警告（故意为之的那些除外） |
| `check_article_examples.py` | 没有指向任何可运行内容的文章，或中英两半指向了不同东西的文章 |
| `check_tests.py` | 定义了却从来没被调用的测试函数，什么都不检查的测试，或者报不出自己失败的测试 |
| `check_test_ports.py` | 两个项目在测试里绑同一个端口，CI 并行跑时就会互相抢 |
| `check_demos.py` | 一启动就崩掉的 demo，或者什么都没做的 demo |
| `check_overview_parity.py` | 中英两份路线图索引列出的阶段或项目不一致 |
| `build_all_projects.py` | 配置、构建或测试失败的项目 |
| `run_sanitizers.py` | 读了已释放内存、越界写入或泄漏，却仍然跑通的测试 |

后两个脚本会真的跑项目，其余脚本只读仓库：

```bash
python3 tools/build_all_projects.py --jobs 4
python3 tools/run_sanitizers.py --jobs 4
```

`run_sanitizers.py` 会用 `-fsanitize=address,undefined` 把每个项目再编一遍，
普通构建放过去的泄漏和 use-after-free，在它这里会被报出来。它在本仓库里
查出了三个真实缺陷：一个还在使用中就被释放的控制块、一份从不释放节点的
链表，以及一对互相持有的对象。

同一个脚本换个参数，问的是另一个问题：

```bash
python3 tools/run_sanitizers.py --tsan --jobs 4
```

ThreadSanitizer 找的是"两个线程没加锁就碰同一块内存"。它查出了三个真实
缺陷：一个线程还在读、另一个线程已经关掉的描述符；在拥有它的交换发生
之前就被移走的节点载荷；以及一个活得比它所属对象还久的定时线程。

两个示例检查器回答的是示例身上的同一个问题：

```bash
python3 tools/check_examples.py --sanitize --jobs 4
python3 tools/check_doc_examples.py --sanitize
```

示例是教材，示例里的泄漏就是在教泄漏。sanitizer 查出了两处：一处是把
一百万个数累加进 `int` 的有符号溢出，一处是聊天室拥有用户、而每个用户
又拥有聊天室的循环引用。

第二个检查器也跑陷阱文章，其中有两篇展示的是程序撑不住的错误：一个悬垂
引用，一个被 insert 弄失效的迭代器。它们列在 `tools/known_failures.tsv`
里 —— 如果哪天它们换了个别的毛病失败，照样会被报出来。

编译器自己报出来的警告也会检查：

```bash
python3 tools/check_warnings.py --jobs 4
```

警告是编译器已经替你找到的错。有三个示例是故意犯错的 —— 正是它们用来
展示悬垂引用和对象切片长什么样 —— 它们列在 `tools/known_warnings.tsv`
里。其余的都应该是安静的。


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
