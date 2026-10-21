# Bilingual Doc Checker

Checks that the English and Chinese halves of this repository's documentation
agree with each other, and that each half is written in the right language.

## Learning Goals

- Classifying a document's language line by line, so that code blocks and API
  tables full of latin identifiers cannot hide an untranslated body
- Stripping comments and string literals to compare two code blocks as
  programs rather than as text
- Pairing two documents that share no filename convention, using their
  structure instead
- Turning a recurring review chore into a testable unit

## Non-Goals

- Judging prose quality or style
- Translating anything
- Parsing markdown in full (only ATX headings and fenced blocks are handled)
- Touching the filesystem: the library works on strings the caller supplies

## API

```cpp
namespace bilingual_doc_checker {

enum class Language { English, Chinese, Unknown };

struct LanguageReport {
    Language verdict;
    std::size_t chineseLines;  // prose lines dominated by CJK characters
    std::size_t englishLines;  // prose lines dominated by latin words
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

## How Language Detection Works

The obvious way to ask "is this document Chinese?" is to count CJK
characters against latin characters. That does not work here. A document such
as an API reference is mostly table rows like:

```markdown
| `save` | `bool save(const std::string& filename) const` | 保存到文件 |
```

The identifiers are latin, the description is Chinese, and the ratio says
"mostly English" even though the prose is Chinese.

So the check works per line instead. A line is Chinese-dominant when it holds
at least two CJK characters and at least as many CJK characters as latin
*words*. A line without any CJK carries no signal and is not counted either
way. The document is Chinese when Chinese-dominant lines outnumber
English-dominant ones.

Fenced code blocks are removed before counting, so a comment written in the
wrong language never influences the verdict.

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/bilingual_doc_checker_example
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Use in This Repository

The library backs two repository-level scripts:

- `tools/check_docs.py` — the language check over every file, plus a best
  effort pairing of the two halves to compare headings and code
- `tools/check_doc_examples.py` — assembles the complete example of every
  document, compiles it and runs it

Together they catch the failure mode that let 15 files sit in `docs/en/` with
English filenames and Chinese bodies for months.
