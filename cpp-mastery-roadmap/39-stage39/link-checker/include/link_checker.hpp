#pragma once

#include <string>
#include <vector>
#include <unordered_set>

namespace link_checker {

struct LinkRef {
    std::string source;   // file the link came from
    std::string target;   // raw link target (as written)
    std::size_t line;     // 1-based line number
};

struct BrokenLink {
    LinkRef ref;
    std::string reason;   // "file not found", "anchor not found", ...
};

// Extract markdown links from one line: [text](target)
// Skips code spans (backticked) and external http(s)/mailto links.
std::vector<LinkRef> extractFromLine(const std::string& source,
                                      const std::string& line,
                                      std::size_t lineNo);

// Check extracted links against a known set of existing file paths
// (relative paths must exist in knownFiles; external links are ignored).
std::vector<BrokenLink> check(const std::vector<LinkRef>& refs,
                              const std::unordered_set<std::string>& knownFiles);

// Normalize a relative path: collapse "./", resolve "../" lexically.
// Does NOT touch the filesystem; pure string operation.
std::string normalizePath(const std::string& path);

}  // namespace link_checker
