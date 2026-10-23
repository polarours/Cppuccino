# 双语文档检查器

校验本仓库文档的中英两半是否彼此一致，以及每一半是否用正确的语言书写。

## 学习目标

- 逐行判定文档语言，让满屏拉丁标识符的代码块和 API 表格无法掩盖未翻译的正文
- 剥离注释与字符串字面量，按"程序"而非"文本"比较两个代码块
- 用结构而非文件名约定来配对两份文档
- 把重复的评审杂活变成可测试的单元

## 非目标

- 评判文风或行文质量
- 翻译任何内容
- 完整解析 markdown（只处理 ATX 标题与围栏代码块）
- 接触文件系统：库只处理调用方传入的字符串

## 接口

```cpp
#include <cstddef>
#include <string>
#include <vector>

namespace bilingual_doc_checker {

enum class Language { English, Chinese, Unknown };

struct LanguageReport {
    Language verdict;
    std::size_t chineseLines;  // 以 CJK 字符占优的正文行
    std::size_t englishLines;  // 以拉丁单词占优的正文行
};

struct Heading { std::size_t level; std::string text; std::size_t line; };
struct CodeBlock { std::string language; std::string code; std::size_t startLine; };

enum class IssueKind {
    EnglishBodyIsChinese,
    ChineseBodyIsEnglish,
    HeadingCountMismatch,
    HeadingLevelMismatch,
    CodeBlockCountMismatch,
    CodeBlockMismatch,
};

struct Issue { IssueKind kind; std::string file; std::string message; };

std::string stripCodeBlocks(const std::string& markdown);
LanguageReport detectLanguage(const std::string& markdown);
std::vector<Heading> extractHeadings(const std::string& markdown);
std::vector<CodeBlock> extractCodeBlocks(const std::string& markdown);
bool codeEquivalent(const std::string& a, const std::string& b);
bool codeEquivalentIgnoringStrings(const std::string& a, const std::string& b);
std::vector<Issue> comparePair(const std::string& enPath, const std::string& enText,
                                const std::string& zhPath, const std::string& zhText);

}  // namespace bilingual_doc_checker
```

## 语言判定是怎么做的

判断"这篇文档是不是中文"最直白的办法是数 CJK 字符和拉丁字符的比例。
但在本仓库里这行不通。像 API 参考这样的文档，正文大半是表格行：

```markdown
| `save` | `bool save(const std::string& filename) const` | 保存到文件 |
```

标识符是拉丁的，描述是中文，比例算下来"大部分是英文"，可正文明明是中文。

所以判定改成逐行进行：一行满足"CJK 字符至少 2 个"且"CJK 字符数不少于拉丁单词数"时，
判为中文占优；完全没有 CJK 的行不提供语言信号，两边都不计入。
中文占优行多于英文占优行时，整篇判为中文。

计数前会先去掉围栏代码块，因此写错语言的注释不会影响判定结果。

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/bilingual_doc_checker_example
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 在本仓库中的用途

该库支撑两个仓库级脚本：

- `tools/check_docs.py` —— 对每个文件做语言检查，并尽力把中英两半配对，
  比较标题骨架与代码
- `tools/check_doc_examples.py` —— 拼装每篇文档的完整示例，编译并运行

两者合起来覆盖了那个让 15 个文件以英文文件名、中文正文在 `docs/en/` 里
躺了好几个月的失效模式。
