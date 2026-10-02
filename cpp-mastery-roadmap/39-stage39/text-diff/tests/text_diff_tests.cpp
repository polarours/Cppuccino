#include "text_diff.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_identical() {
    auto a = text_diff::splitLines("a\nb\nc");
    auto hunks = text_diff::diffLines(a, a);
    expect(hunks.size() == 3, "3 equal lines");
    expect(text_diff::similarity(hunks) == 1.0, "similarity 1.0");
    for (const auto& h : hunks)
        expect(h.op == text_diff::Op::Equal, "all equal");
}

void test_insert_delete() {
    auto a = text_diff::splitLines("x\ny");
    auto b = text_diff::splitLines("x\nnew\ny\nz");
    auto hunks = text_diff::diffLines(a, b);
    // Expect: Equal x, Insert new, Equal y, Insert z
    expect(hunks.size() == 4, "4 hunks");
    expect(hunks[0].op == text_diff::Op::Equal && hunks[0].line == "x", "x equal");
    expect(hunks[1].op == text_diff::Op::Insert && hunks[1].line == "new", "insert new");
    expect(hunks[2].op == text_diff::Op::Equal && hunks[2].line == "y", "y equal");
    expect(hunks[3].op == text_diff::Op::Insert && hunks[3].line == "z", "insert z");
}

void test_delete_middle() {
    auto a = text_diff::splitLines("keep\ndrop\nkeep2");
    auto b = text_diff::splitLines("keep\nkeep2");
    auto hunks = text_diff::diffLines(a, b);
    expect(hunks.size() == 3, "3 hunks");
    expect(hunks[1].op == text_diff::Op::Delete && hunks[1].line == "drop", "drop deleted");
}

void test_empty_sides() {
    auto hunks = text_diff::diffLines({}, text_diff::splitLines("a\nb"));
    expect(hunks.size() == 2, "2 inserts from empty");
    expect(hunks[0].op == text_diff::Op::Insert, "insert");
    auto hunks2 = text_diff::diffLines(text_diff::splitLines("a"), {});
    expect(hunks2.size() == 1 && hunks2[0].op == text_diff::Op::Delete, "all deleted");
}

void test_render() {
    auto a = text_diff::splitLines("a\nb");
    auto b = text_diff::splitLines("a\nc");
    std::string out = text_diff::render(text_diff::diffLines(a, b));
    expect(out == "   a\n - b\n + c\n", "render format");
}

void test_similarity_partial() {
    auto hunks = text_diff::diffLines(text_diff::splitLines("a\nb\nc\nd"),
                                       text_diff::splitLines("a\nb\nc\ne"));
    // 3 equal + 1 delete + 1 insert => 3/5
    double s = text_diff::similarity(hunks);
    expect(s > 0.59 && s < 0.61, "similarity == 0.6 (3 equal of 5 hunks)");
}

}  // namespace

int main() {
    try {
        test_identical();
        test_insert_delete();
        test_delete_middle();
        test_empty_sides();
        test_render();
        test_similarity_partial();
        std::cout << "All text_diff tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
