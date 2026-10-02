#include "text_diff.hpp"

#include <algorithm>

namespace text_diff {

std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : text) {
        if (c == '\n') {
            if (!cur.empty() || out.empty()) out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

std::vector<Hunk> diffLines(const std::vector<std::string>& a,
                            const std::vector<std::string>& b) {
    const std::size_t n = a.size(), m = b.size();
    // dp[i][j] = LCS length of a[i:] and b[j:]
    std::vector<std::vector<std::size_t>> dp(n + 1, std::vector<std::size_t>(m + 1, 0));
    for (std::size_t i = n; i-- > 0;) {
        for (std::size_t j = m; j-- > 0;) {
            dp[i][j] = (a[i] == b[j]) ? dp[i + 1][j + 1] + 1
                                      : std::max(dp[i + 1][j], dp[i][j + 1]);
        }
    }
    // Backtrack into an edit script
    std::vector<Hunk> hunks;
    std::size_t i = 0, j = 0;
    while (i < n && j < m) {
        if (a[i] == b[j]) {
            hunks.push_back({Op::Equal, a[i]});
            ++i; ++j;
        } else if (dp[i + 1][j] >= dp[i][j + 1]) {
            hunks.push_back({Op::Delete, a[i]});
            ++i;
        } else {
            hunks.push_back({Op::Insert, b[j]});
            ++j;
        }
    }
    while (i < n) hunks.push_back({Op::Delete, a[i++]});
    while (j < m) hunks.push_back({Op::Insert, b[j++]});
    return hunks;
}

std::string render(const std::vector<Hunk>& hunks) {
    std::string out;
    for (const auto& h : hunks) {
        switch (h.op) {
            case Op::Equal:  out += "   " + h.line + "\n"; break;
            case Op::Insert: out += " + " + h.line + "\n"; break;
            case Op::Delete: out += " - " + h.line + "\n"; break;
        }
    }
    return out;
}

double similarity(const std::vector<Hunk>& hunks) {
    std::size_t eq = 0, total = 0;
    for (const auto& h : hunks) {
        ++total;
        if (h.op == Op::Equal) ++eq;
    }
    if (total == 0) return 1.0;
    return static_cast<double>(eq) / static_cast<double>(total);
}

}  // namespace text_diff
