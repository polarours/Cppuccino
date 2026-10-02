#include "golden_test.hpp"

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_identical_passes() {
    expect(golden::diffLines("a\nb\n", "a\nb\n").empty(), "equal -> no diff");
}

void test_line_diff_reported() {
    auto d = golden::diffLines("a\nb\nc\n", "a\nX\nc\n");
    expect(!d.empty(), "diff produced");
    bool sawLine2 = false;
    for (const auto& l : d)
        if (l.find("line 2:") != std::string::npos) sawLine2 = true;
    expect(sawLine2, "points at line 2");
}

void test_length_mismatch_reported() {
    auto d = golden::diffLines("a\nb\n", "a\n");
    expect(!d.empty(), "missing line detected");
    bool sawMissing = false;
    for (const auto& l : d)
        if (l.find("<missing>") != std::string::npos) sawMissing = true;
    expect(sawMissing, "<missing> marker used");
}

void test_max_diffs_bounds_output() {
    std::string manyE, manyA;
    for (int i = 0; i < 50; ++i) {
        manyE += "e" + std::to_string(i) + "\n";
        manyA += "a" + std::to_string(i) + "\n";
    }
    auto d = golden::diffLines(manyE, manyA, /*maxDiffs=*/3);
    expect(d.size() <= 9, "bounded at 3 * 3 lines (got " +
                              std::to_string(d.size()) + ")");
}

void test_golden_file_roundtrip() {
    const std::string path = "/tmp/cppuccino_golden_test_1.txt";
    std::remove(path.c_str());
    // missing without UPDATE_GOLDEN -> actionable error
    auto r = golden::checkGolden(path, "hello\n");
    expect(r.find("golden file missing") != std::string::npos,
           "missing file explains UPDATE_GOLDEN");
    // create it
    expect(golden::writeFile(path, "hello\n"), "write");
    expect(golden::checkGolden(path, "hello\n").empty(), "now matches");
    // mismatch -> report with diff
    auto bad = golden::checkGolden(path, "goodbye\n");
    expect(bad.find("golden mismatch") != std::string::npos, "reports mismatch");
    expect(bad.find("- hello") != std::string::npos && bad.find("+ goodbye") != std::string::npos,
           "shows both sides");
    std::remove(path.c_str());
}

void test_normalize_rules() {
    auto s = golden::normalize("took 12345ms (v1.2.3)",
                               {{"took 12345ms", "took <T>"}, {"v1.2.3", "v<V>"}});
    expect(s == "took <T> (v<V>)", "both rules applied");
}

void test_read_missing_returns_empty() {
    expect(golden::readFile("/tmp/cppuccino_does_not_exist.txt").empty(),
           "missing file -> empty string");
}

}  // namespace

int main() {
    try {
        test_identical_passes();
        test_line_diff_reported();
        test_length_mismatch_reported();
        test_max_diffs_bounds_output();
        test_golden_file_roundtrip();
        test_normalize_rules();
        test_read_missing_returns_empty();
        std::cout << "All golden_test tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
