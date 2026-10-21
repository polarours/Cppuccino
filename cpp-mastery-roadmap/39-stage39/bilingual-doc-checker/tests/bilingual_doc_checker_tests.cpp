#include "bilingual_doc_checker.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

using namespace bilingual_doc_checker;

const char* kEnglish = R"(# Ownership and Lifetime

## Why It Matters

Resources must be released when the owning object dies.

```cpp
// a comment
int x = 1;
```

## Summary

Use RAII.
)";

const char* kChinese = R"(# 所有权与生命周期

## 为什么重要

资源必须在所有者对象销毁时释放。

```cpp
// 一条注释
int x = 1;
```

## 总结

使用 RAII。
)";

void test_detect_english() {
    auto report = detectLanguage(kEnglish);
    expect(report.verdict == Language::English, "English doc detected as English");
    expect(report.chineseLines == 0, "no Chinese prose lines");
    // A latin-only prose line carries no language signal, so it is not
    // counted either way; the verdict still comes out English.
    expect(report.englishLines == 0, "latin-only lines are not counted");
}

void test_detect_chinese() {
    auto report = detectLanguage(kChinese);
    expect(report.verdict == Language::Chinese, "Chinese doc detected as Chinese");
    expect(report.englishLines == 0, "no English prose lines");
    expect(report.chineseLines > 0, "some Chinese prose lines");
}

void test_code_comments_do_not_affect_language() {
    // A doc whose code block is full of Chinese must still count as English:
    // the code block is not prose.
    const char* doc = R"(# Title

Some English prose here.

```cpp
// 这是一条中文注释
std::string s = "中文字符串";
```
)";
    auto report = detectLanguage(doc);
    expect(report.verdict == Language::English, "code block ignored for language");
    expect(report.chineseLines == 0, "Chinese inside code is not prose");
}

void test_api_table_does_not_hide_chinese_body() {
    // This is the case that a naive character ratio gets wrong: the table is
    // full of latin identifiers, so the body looks English unless the code
    // block and the identifiers are handled per line.
    const char* doc = R"(# API

| Method | Signature | Description |
|---|---|---|
| `save` | `bool save(const std::string& filename) const` | 保存到文件 |

## 概述

`simple-db` 是一个内存数据库，支持表管理和 CRUD 操作。
)";
    auto report = detectLanguage(doc);
    expect(report.verdict == Language::Chinese, "Chinese body behind a latin table");
}

void test_strip_code_blocks() {
    std::string stripped = stripCodeBlocks(kEnglish);
    expect(stripped.find("int x = 1;") == std::string::npos, "code removed");
    expect(stripped.find("Resources must be released") != std::string::npos,
           "prose kept");
}

void test_extract_headings() {
    auto headings = extractHeadings(kEnglish);
    expect(headings.size() == 3, "three headings");
    expect(headings[0].level == 1, "first is level 1");
    expect(headings[0].text == "Ownership and Lifetime", "title text");
    expect(headings[1].level == 2, "second is level 2");
    expect(headings[1].line > 1, "line number tracked");
}

void test_headings_inside_code_block_ignored() {
    const char* doc = R"(# Title

```cpp
// # not a heading
## also not a heading
```
)";
    auto headings = extractHeadings(doc);
    expect(headings.size() == 1, "only the real heading");
}

void test_extract_code_blocks() {
    auto blocks = extractCodeBlocks(kEnglish);
    expect(blocks.size() == 1, "one code block");
    expect(blocks[0].language == "cpp", "info string captured");
    expect(blocks[0].code.find("int x = 1;") != std::string::npos, "body captured");
    expect(blocks[0].code.find("```") == std::string::npos, "fences excluded");
}

void test_code_equivalent_ignores_comments() {
    expect(codeEquivalent("// hello\nint x = 1;", "int x = 1;"), "comment ignored");
    expect(codeEquivalent("/* a */ int x;", "int x;"), "block comment ignored");
    expect(codeEquivalent("int x = 1;", "int  x  =  1;"), "whitespace ignored");
    expect(!codeEquivalent("int x = 1;", "int x = 2;"), "different code differs");
}

void test_code_equivalent_respects_string_literals() {
    // A "//" inside a string is not a comment.
    expect(codeEquivalent("const char* u = \"http://x\";", "const char* u = \"http://x\";"),
           "url string kept");
    expect(!codeEquivalent("const char* u = \"http://x\";", "const char* u = \"https://x\";"),
           "different url differs");
}

void test_code_equivalent_ignoring_strings() {
    // A translated document prints its messages in its own language, so the
    // message text may differ while the program stays the same.
    expect(codeEquivalentIgnoringStrings("std::cout << \"Hello\";", "std::cout << \"你好\";"),
           "translated message accepted");
    expect(!codeEquivalentIgnoringStrings("int x = 1;", "int x = 2;"),
           "different code still differs");
    expect(codeEquivalentIgnoringStrings("// note\nint x = 1;", "int x = 1;"),
           "comments ignored as well");
}

void test_compare_pair_clean() {
    auto issues = comparePair("en.md", kEnglish, "zh.md", kChinese);
    expect(issues.empty(), "matching pair has no issues");
}

void test_compare_pair_detects_chinese_english_body() {
    // The English slot holds a Chinese document: exactly the bug that let
    // 15 files slip through with English filenames.
    auto issues = comparePair("en.md", kChinese, "zh.md", kChinese);
    expect(issues.size() == 1, "one issue");
    expect(issues[0].kind == IssueKind::EnglishBodyIsChinese, "right kind");
    expect(issues[0].file == "en.md", "reported against the English file");
}

void test_compare_pair_detects_heading_drift() {
    std::string zh = kChinese;
    // Drop one heading from the Chinese side.
    std::string::size_type pos = zh.find("## 总结");
    zh.erase(pos, zh.find('\n', pos) - pos + 1);
    auto issues = comparePair("en.md", kEnglish, "zh.md", zh);
    expect(issues.size() == 1, "one issue");
    expect(issues[0].kind == IssueKind::HeadingCountMismatch, "right kind");
}

void test_compare_pair_detects_code_drift() {
    std::string zh = kChinese;
    std::string::size_type pos = zh.find("int x = 1;");
    zh.replace(pos, 9, "int x = 2;");
    auto issues = comparePair("en.md", kEnglish, "zh.md", zh);
    expect(issues.size() == 1, "one issue");
    expect(issues[0].kind == IssueKind::CodeBlockMismatch, "right kind");
}

void test_compare_pair_detects_code_count_drift() {
    std::string zh = kChinese;
    zh += "\n```cpp\nint y = 2;\n```\n";
    auto issues = comparePair("en.md", kEnglish, "zh.md", zh);
    expect(issues.size() == 1, "one issue");
    expect(issues[0].kind == IssueKind::CodeBlockCountMismatch, "right kind");
}

void test_to_string_covers_every_kind() {
    IssueKind all[] = {
        IssueKind::EnglishBodyIsChinese, IssueKind::ChineseBodyIsEnglish,
        IssueKind::HeadingCountMismatch, IssueKind::HeadingLevelMismatch,
        IssueKind::CodeBlockCountMismatch, IssueKind::CodeBlockMismatch,
    };
    for (IssueKind kind : all) {
        expect(std::string(toString(kind)) != "unknown", "kind has a name");
    }
}

}  // namespace

int main() {
    try {
        test_detect_english();
        test_detect_chinese();
        test_code_comments_do_not_affect_language();
        test_api_table_does_not_hide_chinese_body();
        test_strip_code_blocks();
        test_extract_headings();
        test_headings_inside_code_block_ignored();
        test_extract_code_blocks();
        test_code_equivalent_ignores_comments();
        test_code_equivalent_respects_string_literals();
        test_code_equivalent_ignoring_strings();
        test_compare_pair_clean();
        test_compare_pair_detects_chinese_english_body();
        test_compare_pair_detects_heading_drift();
        test_compare_pair_detects_code_drift();
        test_compare_pair_detects_code_count_drift();
        test_to_string_covers_every_kind();
        std::cout << "All bilingual_doc_checker tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
