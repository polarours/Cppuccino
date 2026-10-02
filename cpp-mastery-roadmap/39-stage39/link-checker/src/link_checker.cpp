#include "link_checker.hpp"

#include <algorithm>
#include <sstream>

namespace link_checker {

namespace {

bool external(const std::string& t) {
    return t.rfind("http://", 0) == 0 || t.rfind("https://", 0) == 0 ||
           t.rfind("mailto:", 0) == 0 || (!t.empty() && t[0] == '#');
}

}  // namespace

std::vector<LinkRef> extractFromLine(const std::string& source,
                                      const std::string& line,
                                      std::size_t lineNo) {
    std::vector<LinkRef> out;
    // Skip inline code spans so `[](x)` inside code is ignored
    std::string cleaned;
    bool inCode = false;
    for (char c : line) {
        if (c == '`') { inCode = !inCode; continue; }
        if (!inCode) cleaned.push_back(c);
    }
    std::size_t pos = 0;
    while ((pos = cleaned.find("](", pos)) != std::string::npos) {
        std::size_t open = cleaned.rfind('[', pos);
        std::size_t close = cleaned.find(')', pos + 2);
        if (open == std::string::npos || close == std::string::npos) break;
        out.push_back({source, cleaned.substr(pos + 2, close - pos - 2), lineNo});
        pos = close + 1;
    }
    return out;
}

std::string normalizePath(const std::string& path) {
    std::vector<std::string> parts;
    bool absolute = !path.empty() && path[0] == '/';
    std::istringstream in(path);
    std::string seg;
    while (std::getline(in, seg, '/')) {
        if (seg.empty() || seg == ".") continue;
        if (seg == "..") {
            if (!parts.empty() && parts.back() != "..") parts.pop_back();
            else if (!absolute) parts.push_back("..");
        } else {
            parts.push_back(seg);
        }
    }
    std::string out = absolute ? "/" : "";
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i) out += "/";
        out += parts[i];
    }
    if (out.empty()) out = absolute ? "/" : ".";
    return out;
}

std::vector<BrokenLink> check(const std::vector<LinkRef>& refs,
                              const std::unordered_set<std::string>& knownFiles) {
    std::vector<BrokenLink> broken;
    for (const auto& r : refs) {
        if (external(r.target)) continue;
        std::string target = r.target;
        std::string anchor;
        auto hash = target.find('#');
        if (hash != std::string::npos) {
            anchor = target.substr(hash + 1);
            target = target.substr(0, hash);
        }
        if (target.empty()) continue;  // pure in-page anchor
        std::string norm = normalizePath(target);
        if (knownFiles.count(norm) == 0) {
            broken.push_back({r, "file not found: " + norm});
            continue;
        }
        // Anchor check: only when caller tracks anchors ("file#anchor" entries)
        if (!anchor.empty()) {
            bool tracks = std::any_of(knownFiles.begin(), knownFiles.end(),
                                      [&](const std::string& k) {
                                          return k.rfind(norm + "#", 0) == 0;
                                      });
            if (tracks && knownFiles.count(norm + "#" + anchor) == 0) {
                broken.push_back({r, "anchor not found: #" + anchor});
            }
        }
    }
    return broken;
}

}  // namespace link_checker
