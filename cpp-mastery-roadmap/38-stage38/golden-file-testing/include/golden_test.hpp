#pragma once

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace golden {

// Compare actual vs expected line-by-line; returns human-readable diffs
// (first N mismatches, N = maxDiffs).
inline std::vector<std::string> diffLines(const std::string& expected,
                                          const std::string& actual,
                                          std::size_t maxDiffs = 5) {
    auto split = [](const std::string& s) {
        std::vector<std::string> out;
        std::istringstream in(s);
        std::string line;
        while (std::getline(in, line)) out.push_back(line);
        return out;
    };
    auto e = split(expected);
    auto a = split(actual);

    std::vector<std::string> diffs;
    std::size_t n = std::max(e.size(), a.size());
    for (std::size_t i = 0; i < n && diffs.size() < maxDiffs * 3; ++i) {
        const std::string ev = i < e.size() ? e[i] : "<missing>";
        const std::string av = i < a.size() ? a[i] : "<missing>";
        if (ev != av) {
            diffs.push_back("line " + std::to_string(i + 1) + ":");
            diffs.push_back("  - " + ev);
            diffs.push_back("  + " + av);
        }
    }
    return diffs;
}

inline std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return {};
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

inline bool writeFile(const std::string& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out << content;
    return static_cast<bool>(out);
}

// The assertion helper:
//   - If the golden file does not exist and UPDATE_GOLDEN env is set,
//     create it (acceptance run).
//   - Otherwise compare and return "" on match, or a diff report.
// Never aborts itself - callers decide how to fail (throw / return code),
// which keeps the helper usable outside test frameworks.
inline std::string checkGolden(const std::string& goldenPath,
                               const std::string& actual) {
    std::string expected = readFile(goldenPath);
    if (expected.empty() && !std::ifstream(goldenPath)) {
        if (std::getenv("UPDATE_GOLDEN")) {
            return writeFile(goldenPath, actual)
                       ? ""
                       : "cannot create golden file: " + goldenPath;
        }
        return "golden file missing: " + goldenPath +
               " (run with UPDATE_GOLDEN=1 to create)";
    }
    if (expected == actual) return "";
    auto diffs = diffLines(expected, actual);
    std::string report = "golden mismatch: " + goldenPath + "\n";
    for (const auto& d : diffs) report += d + "\n";
    if (diffs.empty())
        report += "  (content differs but no per-line diff; sizes " +
                  std::to_string(expected.size()) + " vs " +
                  std::to_string(actual.size()) + ")";
    return report;
}

// Normalize volatile output before comparison (timestamps, paths, ...).
inline std::string normalize(std::string s,
                             const std::vector<std::pair<std::string, std::string>>& rules = {}) {
    for (const auto& [pattern, replacement] : rules) {
        std::size_t pos = 0;
        while ((pos = s.find(pattern, pos)) != std::string::npos) {
            s.replace(pos, pattern.size(), replacement);
            pos += replacement.size();
        }
    }
    return s;
}

}  // namespace golden
