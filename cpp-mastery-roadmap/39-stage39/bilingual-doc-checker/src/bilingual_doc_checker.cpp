#include "bilingual_doc_checker.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace bilingual_doc_checker {

namespace {

// Is this byte the start of a UTF-8 encoded CJK ideograph?
bool isCjkStart(unsigned char c) {
    return c >= 0xE4 && c <= 0xE9;  // U+4E00 .. U+9FFF, first byte of 3-byte seq
}

// Length in bytes of the UTF-8 sequence starting at s[i].
std::size_t utf8Length(const std::string& s, std::size_t i) {
    unsigned char c = static_cast<unsigned char>(s[i]);
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// Count CJK characters (U+4E00 .. U+9FFF) in a line.
std::size_t countCjk(const std::string& line) {
    std::size_t count = 0;
    for (std::size_t i = 0; i < line.size();) {
        unsigned char c = static_cast<unsigned char>(line[i]);
        if (isCjkStart(c)) {
            // The second byte of a CJK sequence is in 0x80..0xBF; the third
            // too. Requiring both bounds the range to U+4E00..U+9FFF.
            std::size_t len = utf8Length(line, i);
            if (len == 3 && i + 2 < line.size()) {
                unsigned char b2 = static_cast<unsigned char>(line[i + 1]);
                unsigned char b3 = static_cast<unsigned char>(line[i + 2]);
                if (b2 >= 0x80 && b2 <= 0xBF && b3 >= 0x80 && b3 <= 0xBF) {
                    ++count;
                    i += 3;
                    continue;
                }
            }
        }
        i += utf8Length(line, i);
    }
    return count;
}

// Count runs of latin letters, which approximate "words".
std::size_t countWords(const std::string& line) {
    std::size_t words = 0;
    bool inWord = false;
    for (char c : line) {
        bool alpha = std::isalpha(static_cast<unsigned char>(c)) != 0;
        if (alpha && !inWord) {
            ++words;
            inWord = true;
        } else if (!alpha) {
            inWord = false;
        }
    }
    return words;
}

// The fence marker of a line, or an empty string when the line is not a fence.
std::string fenceMarker(const std::string& line) {
    std::size_t i = 0;
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
    if (i >= line.size()) return "";
    char c = line[i];
    if (c != '`' && c != '~') return "";
    std::size_t run = 0;
    while (i + run < line.size() && line[i + run] == c) ++run;
    if (run < 3) return "";
    return line.substr(i, run);
}

// Strip // and /* */ comments from C++ source, respecting string literals.
std::string stripComments(const std::string& src) {
    std::string out;
    out.reserve(src.size());
    std::size_t i = 0;
    const std::size_t n = src.size();
    bool inString = false;
    bool inChar = false;
    while (i < n) {
        char c = src[i];
        if (inString) {
            out.push_back(c);
            if (c == '\\' && i + 1 < n) {
                out.push_back(src[i + 1]);
                i += 2;
                continue;
            }
            if (c == '"') inString = false;
            ++i;
            continue;
        }
        if (inChar) {
            out.push_back(c);
            if (c == '\\' && i + 1 < n) {
                out.push_back(src[i + 1]);
                i += 2;
                continue;
            }
            if (c == '\'') inChar = false;
            ++i;
            continue;
        }
        if (c == '"') {
            inString = true;
            out.push_back(c);
            ++i;
            continue;
        }
        if (c == '\'') {
            inChar = true;
            out.push_back(c);
            ++i;
            continue;
        }
        if (c == '/' && i + 1 < n && src[i + 1] == '/') {
            while (i < n && src[i] != '\n') ++i;
            continue;
        }
        if (c == '/' && i + 1 < n && src[i + 1] == '*') {
            i += 2;
            while (i + 1 < n && !(src[i] == '*' && src[i + 1] == '/')) ++i;
            i = (i + 1 < n) ? i + 2 : n;
            out.push_back(' ');
            continue;
        }
        out.push_back(c);
        ++i;
    }
    return out;
}

// Replace the contents of every string literal with a placeholder, so that
// two programs that only differ in their printed text still compare equal.
std::string blankStrings(const std::string& src) {
    std::string out;
    out.reserve(src.size());
    std::size_t i = 0;
    const std::size_t n = src.size();
    bool inString = false;
    bool inChar = false;
    while (i < n) {
        char c = src[i];
        if (inString) {
            if (c == '\\' && i + 1 < n) {
                i += 2;
                continue;
            }
            if (c == '"') {
                inString = false;
                out.push_back(c);
            }
            ++i;
            continue;
        }
        if (inChar) {
            if (c == '\\' && i + 1 < n) {
                i += 2;
                continue;
            }
            if (c == '\'') {
                inChar = false;
                out.push_back(c);
            }
            ++i;
            continue;
        }
        if (c == '"') {
            inString = true;
            out.push_back(c);
            ++i;
            continue;
        }
        if (c == '\'') {
            inChar = true;
            out.push_back(c);
            ++i;
            continue;
        }
        out.push_back(c);
        ++i;
    }
    return out;
}

// Collapse whitespace so that formatting differences do not count as code
// differences.
std::string normalizeWhitespace(const std::string& s) {
    std::string out;
    bool pendingSpace = false;
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            pendingSpace = !out.empty();
            continue;
        }
        if (pendingSpace) {
            out.push_back(' ');
            pendingSpace = false;
        }
        out.push_back(c);
    }
    return out;
}

std::string trim(const std::string& s) {
    std::size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    std::size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

}  // namespace

const char* toString(IssueKind kind) {
    switch (kind) {
        case IssueKind::EnglishBodyIsChinese: return "english body is chinese";
        case IssueKind::ChineseBodyIsEnglish: return "chinese body is english";
        case IssueKind::HeadingCountMismatch: return "heading count mismatch";
        case IssueKind::HeadingLevelMismatch: return "heading level mismatch";
        case IssueKind::CodeBlockCountMismatch: return "code block count mismatch";
        case IssueKind::CodeBlockMismatch: return "code block mismatch";
    }
    return "unknown";
}

std::string stripCodeBlocks(const std::string& markdown) {
    std::istringstream in(markdown);
    std::string line;
    std::string out;
    std::string openFence;
    while (std::getline(in, line)) {
        std::string marker = fenceMarker(line);
        if (openFence.empty()) {
            if (!marker.empty()) {
                openFence = marker;
                continue;  // drop the opening fence
            }
            out += line;
            out += '\n';
        } else {
            // Inside a block: only a matching fence closes it.
            if (marker.size() >= openFence.size() &&
                marker.compare(0, openFence.size(), openFence) == 0) {
                openFence.clear();
            }
        }
    }
    return out;
}

LanguageReport detectLanguage(const std::string& markdown) {
    LanguageReport report;
    std::istringstream in(stripCodeBlocks(markdown));
    std::string line;
    std::size_t proseLines = 0;
    while (std::getline(in, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        ++proseLines;
        std::size_t cjk = countCjk(line);
        // A line without any CJK carries no signal about which language the
        // document is written in, so it is not counted either way.
        if (cjk == 0) continue;
        std::size_t words = countWords(line);
        if (cjk >= std::max<std::size_t>(2, words)) {
            ++report.chineseLines;
        } else {
            ++report.englishLines;
        }
    }
    if (proseLines == 0) {
        report.verdict = Language::Unknown;
    } else {
        report.verdict =
            report.chineseLines > report.englishLines ? Language::Chinese
                                                      : Language::English;
    }
    return report;
}

std::vector<Heading> extractHeadings(const std::string& markdown) {
    std::vector<Heading> headings;
    std::istringstream in(markdown);
    std::string line;
    std::size_t lineNo = 0;
    std::string openFence;
    while (std::getline(in, line)) {
        ++lineNo;
        std::string marker = fenceMarker(line);
        if (!openFence.empty()) {
            if (marker.size() >= openFence.size() &&
                marker.compare(0, openFence.size(), openFence) == 0) {
                openFence.clear();
            }
            continue;
        }
        if (!marker.empty()) {
            openFence = marker;
            continue;
        }
        std::size_t i = 0;
        while (i < line.size() && line[i] == ' ') ++i;
        if (i >= line.size() || line[i] != '#') continue;
        std::size_t level = 0;
        while (i < line.size() && line[i] == '#') {
            ++level;
            ++i;
        }
        if (level == 0 || level > 6) continue;
        if (i < line.size() && line[i] != ' ' && line[i] != '\t') continue;
        headings.push_back({level, trim(line.substr(i)), lineNo});
    }
    return headings;
}

std::vector<CodeBlock> extractCodeBlocks(const std::string& markdown) {
    std::vector<CodeBlock> blocks;
    std::istringstream in(markdown);
    std::string line;
    std::size_t lineNo = 0;
    std::string openFence;
    std::string openInfo;
    std::size_t openLine = 0;
    std::string body;
    while (std::getline(in, line)) {
        ++lineNo;
        std::string marker = fenceMarker(line);
        if (openFence.empty()) {
            if (!marker.empty()) {
                openFence = marker;
                openLine = lineNo;
                // The info string is the rest of the opening fence line.
                std::size_t p = line.find(marker);
                openInfo = trim(line.substr(p + marker.size()));
                body.clear();
            }
            continue;
        }
        if (marker.size() >= openFence.size() &&
            marker.compare(0, openFence.size(), openFence) == 0) {
            blocks.push_back({openInfo, body, openLine});
            openFence.clear();
            openInfo.clear();
            body.clear();
            continue;
        }
        body += line;
        body += '\n';
    }
    return blocks;
}

bool codeEquivalent(const std::string& a, const std::string& b) {
    return normalizeWhitespace(stripComments(a)) ==
           normalizeWhitespace(stripComments(b));
}

bool codeEquivalentIgnoringStrings(const std::string& a, const std::string& b) {
    return normalizeWhitespace(blankStrings(stripComments(a))) ==
           normalizeWhitespace(blankStrings(stripComments(b)));
}

std::vector<Issue> comparePair(const std::string& enPath,
                                const std::string& enText,
                                const std::string& zhPath,
                                const std::string& zhText) {
    std::vector<Issue> issues;

    LanguageReport en = detectLanguage(enText);
    if (en.verdict == Language::Chinese) {
        issues.push_back({IssueKind::EnglishBodyIsChinese, enPath,
                          "expected an English body but found " +
                              std::to_string(en.chineseLines) +
                              " Chinese-dominant prose lines"});
    }

    LanguageReport zh = detectLanguage(zhText);
    if (zh.verdict == Language::English) {
        issues.push_back({IssueKind::ChineseBodyIsEnglish, zhPath,
                          "expected a Chinese body but found " +
                              std::to_string(zh.englishLines) +
                              " English-dominant prose lines"});
    }

    std::vector<Heading> enHeadings = extractHeadings(enText);
    std::vector<Heading> zhHeadings = extractHeadings(zhText);
    if (enHeadings.size() != zhHeadings.size()) {
        issues.push_back({IssueKind::HeadingCountMismatch, enPath,
                          "English has " + std::to_string(enHeadings.size()) +
                              " headings, Chinese has " +
                              std::to_string(zhHeadings.size())});
    } else {
        for (std::size_t i = 0; i < enHeadings.size(); ++i) {
            if (enHeadings[i].level != zhHeadings[i].level) {
                issues.push_back({IssueKind::HeadingLevelMismatch, enPath,
                                  "heading " + std::to_string(i + 1) +
                                      " is level " +
                                      std::to_string(enHeadings[i].level) +
                                      " in English but level " +
                                      std::to_string(zhHeadings[i].level) +
                                      " in Chinese"});
            }
        }
    }

    std::vector<CodeBlock> enBlocks = extractCodeBlocks(enText);
    std::vector<CodeBlock> zhBlocks = extractCodeBlocks(zhText);
    if (enBlocks.size() != zhBlocks.size()) {
        issues.push_back({IssueKind::CodeBlockCountMismatch, enPath,
                          "English has " + std::to_string(enBlocks.size()) +
                              " code blocks, Chinese has " +
                              std::to_string(zhBlocks.size())});
    } else {
        for (std::size_t i = 0; i < enBlocks.size(); ++i) {
            // A translated document prints its messages in its own language,
            // so string literal contents are allowed to differ.
            if (!codeEquivalentIgnoringStrings(enBlocks[i].code, zhBlocks[i].code)) {
                issues.push_back({IssueKind::CodeBlockMismatch, enPath,
                                  "code block " + std::to_string(i + 1) +
                                      " differs beyond its comments and messages"});
            }
        }
    }

    return issues;
}

}  // namespace bilingual_doc_checker
