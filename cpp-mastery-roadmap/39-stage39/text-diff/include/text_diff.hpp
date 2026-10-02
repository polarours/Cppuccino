#pragma once

#include <string>
#include <vector>

namespace text_diff {

enum class Op { Equal, Insert, Delete };

struct Hunk {
    Op op;
    std::string line;
};

// Line-based diff via LCS (classic O(n*m) dynamic programming).
// Returns edit script: Equal lines kept, Delete removed, Insert added.
std::vector<Hunk> diffLines(const std::vector<std::string>& a,
                            const std::vector<std::string>& b);

// Split text on '\n' (a trailing newline does not produce an empty last line).
std::vector<std::string> splitLines(const std::string& text);

// Unified-diff-ish rendering: "- " deleted, "+ " added, "  " equal.
std::string render(const std::vector<Hunk>& hunks);

// Similarity ratio in [0,1]: equal lines / max(total lines).
double similarity(const std::vector<Hunk>& hunks);

}  // namespace text_diff
