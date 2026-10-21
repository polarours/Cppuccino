#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace bilingual_doc_checker {

// Which language a document's prose is written in.
enum class Language {
    English,
    Chinese,
    Unknown,  // no prose at all
};

// Result of classifying a document body.
struct LanguageReport {
    Language verdict = Language::Unknown;
    std::size_t chineseLines = 0;  // prose lines dominated by CJK characters
    std::size_t englishLines = 0;  // prose lines dominated by latin words
};

// A markdown heading.
struct Heading {
    std::size_t level = 0;  // 1 for "#", 2 for "##", ...
    std::string text;
    std::size_t line = 0;  // 1-based line number
};

// A fenced code block.
struct CodeBlock {
    std::string language;  // the info string, e.g. "cpp"; empty when untagged
    std::string code;      // the block body, fences excluded
    std::size_t startLine = 0;
};

// Everything that can be wrong with one half of a bilingual pair.
enum class IssueKind {
    EnglishBodyIsChinese,   // docs/en file whose prose is still Chinese
    ChineseBodyIsEnglish,   // docs/中文版 file whose prose is English
    HeadingCountMismatch,    // the two halves have a different number of headings
    HeadingLevelMismatch,    // the two halves disagree on a heading level
    CodeBlockCountMismatch,  // the two halves have a different number of code blocks
    CodeBlockMismatch,       // a code block differs beyond its comments
};

const char* toString(IssueKind kind);

// One problem found in a document pair.
struct Issue {
    IssueKind kind = IssueKind::EnglishBodyIsChinese;
    std::string file;     // the file the issue is about
    std::string message;  // human readable detail
};

// Remove fenced code blocks, keeping the prose around them.
// A fence opened with ``` or ~~~ runs until a matching closing fence.
std::string stripCodeBlocks(const std::string& markdown);

// Classify the prose of a document.
//
// Every prose line is counted as Chinese-dominant or English-dominant: a line
// is Chinese-dominant when it holds at least two CJK characters and at least
// as many CJK characters as latin words. The document is Chinese when
// Chinese-dominant lines outnumber English-dominant ones.
//
// The per-line rule matters because code blocks and API tables are full of
// latin identifiers, which inflate a naive character-ratio count and hide an
// untranslated body.
LanguageReport detectLanguage(const std::string& markdown);

// Every ATX heading in the document, in order.
std::vector<Heading> extractHeadings(const std::string& markdown);

// Every fenced code block in the document, in order.
std::vector<CodeBlock> extractCodeBlocks(const std::string& markdown);

// Compare two code blocks ignoring comments and whitespace.
//
// Comments are recognised in both // and /* */ form, and string literals are
// respected so that a "//" inside a string is not mistaken for a comment.
bool codeEquivalent(const std::string& a, const std::string& b);

// Compare two code blocks the way a translation pair should match.
//
// On top of codeEquivalent this also ignores the contents of string
// literals, because a translated document prints its messages in the
// document's own language while running the very same program.
bool codeEquivalentIgnoringStrings(const std::string& a, const std::string& b);

// Compare an English document with its Chinese twin.
//
// Returns one issue per problem found; an empty result means the pair agrees
// on language, heading structure and code.
std::vector<Issue> comparePair(const std::string& enPath,
                                const std::string& enText,
                                const std::string& zhPath,
                                const std::string& zhText);

}  // namespace bilingual_doc_checker
