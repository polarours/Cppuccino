#include "link_checker.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_extract_simple() {
    auto refs = link_checker::extractFromLine("a.md", "see [x](dir/file.md)", 7);
    expect(refs.size() == 1, "one link");
    expect(refs[0].target == "dir/file.md", "target");
    expect(refs[0].line == 7, "line number");
}

void test_skip_external_and_code() {
    auto refs = link_checker::extractFromLine(
        "a.md", "`[fake](nope.md)` real [ok](yes.md) [web](https://x.io)", 1);
    expect(refs.size() == 2, "code span skipped, two links remain");
    expect(refs[0].target == "yes.md", "first is yes.md");
}

void test_normalize() {
    expect(link_checker::normalizePath("a/../b/./c") == "b/c", "dot-dot and dot");
    expect(link_checker::normalizePath("./x/y") == "x/y", "leading ./");
    expect(link_checker::normalizePath("a/b/../../..") == "..", "above root");
    expect(link_checker::normalizePath("/x//y") == "/x/y", "collapse slashes");
}

void test_broken_file() {
    std::unordered_set<std::string> known = {"docs/guide.md"};
    auto refs = link_checker::extractFromLine(
        "README.md", "[ok](docs/guide.md) [bad](docs/missing.md)", 3);
    auto broken = link_checker::check(refs, known);
    expect(broken.size() == 1, "only one broken");
    expect(broken[0].ref.target == "docs/missing.md", "missing.md flagged");
}

void test_external_ignored() {
    std::unordered_set<std::string> known;
    auto refs = link_checker::extractFromLine(
        "a.md", "[a](https://x.io) [b](mailto:y@z) [c](#section)", 1);
    auto broken = link_checker::check(refs, known);
    expect(broken.empty(), "external/pure-anchor links never broken");
}

void test_anchor_check() {
    // Caller tracks anchors for README.md via "README.md#intro" entries
    std::unordered_set<std::string> known = {"README.md", "README.md#intro"};
    auto refs = link_checker::extractFromLine(
        "x.md", "[a](README.md#intro) [b](README.md#missing)", 1);
    auto broken = link_checker::check(refs, known);
    expect(broken.size() == 1, "only missing anchor flagged");
    expect(broken[0].reason.find("#missing") != std::string::npos,
           "reason mentions the anchor");
}

}  // namespace

int main() {
    try {
        test_extract_simple();
        test_skip_external_and_code();
        test_normalize();
        test_broken_file();
        test_external_ignored();
        test_anchor_check();
        std::cout << "All link_checker tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
