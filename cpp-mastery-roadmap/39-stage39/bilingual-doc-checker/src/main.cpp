#include "bilingual_doc_checker.hpp"

#include <iostream>
#include <string>

int main() {
    // A pair that is out of sync: the "English" file still holds Chinese
    // prose, and the Chinese half has an extra heading.
    const std::string en =
        "# 所有权与生命周期\n"
        "\n"
        "## 为什么重要\n"
        "\n"
        "资源必须在所有者对象销毁时释放。\n"
        "\n"
        "```cpp\n"
        "int x = 1;\n"
        "```\n"
        "\n"
        "## 总结\n"
        "\n"
        "使用 RAII。\n";

    const std::string zh =
        "# 所有权与生命周期\n"
        "\n"
        "## 为什么重要\n"
        "\n"
        "资源必须在所有者对象销毁时释放。\n"
        "\n"
        "```cpp\n"
        "int x = 1;\n"
        "```\n"
        "\n"
        "## 总结\n"
        "\n"
        "使用 RAII。\n"
        "\n"
        "## 另请参阅\n"
        "\n"
        "- [RAII 与异常安全](RAII与异常安全.md)\n";

    auto issues = bilingual_doc_checker::comparePair("docs/en/ownership.md", en,
                                                     "docs/中文版/所有权.md", zh);
    std::cout << "found " << issues.size() << " issue(s)\n";
    for (const auto& issue : issues) {
        std::cout << "  " << issue.file << ": " << bilingual_doc_checker::toString(issue.kind)
                  << " -- " << issue.message << "\n";
    }

    // The language verdict is available on its own.
    auto report = bilingual_doc_checker::detectLanguage(en);
    std::cout << "verdict: "
              << (report.verdict == bilingual_doc_checker::Language::Chinese
                      ? "chinese"
                      : "english")
              << " (" << report.chineseLines << " chinese lines, "
              << report.englishLines << " english lines)\n";

    return 0;
}
