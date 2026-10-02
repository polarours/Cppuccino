#include "link_checker.hpp"

#include <iostream>
#include <unordered_set>

int main() {
    const std::string line =
        "See [plan](../DEVELOPMENT_PLAN.md) and [site](https://example.com) "
        "and [broken](missing/file.md)";

    std::unordered_set<std::string> known = {"../DEVELOPMENT_PLAN.md"};

    auto refs = link_checker::extractFromLine("README.md", line, 1);
    std::cout << "extracted " << refs.size() << " links\n";

    auto broken = link_checker::check(refs, known);
    for (const auto& b : broken)
        std::cout << "BROKEN at line " << b.ref.line << ": "
                  << b.ref.target << " (" << b.reason << ")\n";

    std::cout << "normalize a/../b/./c = "
              << link_checker::normalizePath("a/../b/./c") << "\n";
    return 0;
}
