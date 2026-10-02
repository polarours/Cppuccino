#include "trie.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_insert_contains() {
    trie::Trie t;
    t.insert("hello");
    t.insert("world");
    expect(t.contains("hello"), "hello present");
    expect(t.contains("world"), "world present");
    expect(!t.contains("hell"), "prefix is not a word");
    expect(!t.contains("HELLO"), "case sensitive");
    expect(t.size() == 2, "two unique words");
}

void test_duplicates_count_once() {
    trie::Trie t;
    t.insert("aa");
    t.insert("aa");
    expect(t.size() == 1, "duplicates do not increase unique count");
    expect(t.contains("aa"), "still contains");
}

void test_startswith() {
    trie::Trie t;
    t.insert("prefixed");
    expect(t.startsWith("pref"), "prefix found");
    expect(!t.startsWith("suff"), "wrong prefix");
    expect(!t.contains("pref"), "prefix not a full word");
    expect(t.contains("prefixed"), "full word still found");
}

void test_count_prefix() {
    trie::Trie t;
    for (auto* w : {"car", "card", "care", "dog"}) t.insert(w);
    expect(t.countWordsWithPrefix("car") == 3, "car, card, care");
    expect(t.countWordsWithPrefix("ca") == 3, "all car-family");
    expect(t.countWordsWithPrefix("z") == 0, "missing prefix -> 0");
}

void test_words_with_prefix_sorted() {
    trie::Trie t;
    for (auto* w : {"care", "car", "card", "cat"}) t.insert(w);
    auto words = t.wordsWithPrefix("car");
    std::sort(words.begin(), words.end());  // collect already emits lex order
    expect(words.size() == 3, "3 words");
    expect((words == std::vector<std::string>{"car", "card", "care"}),
           "car family listed");
    expect(t.wordsWithPrefix("cart").empty(), "no words for unknown prefix");
}

void test_empty_string_edge() {
    trie::Trie t;
    expect(!t.contains(""), "empty not inserted");
    // Empty prefix descends to the root -> matches (vacuous truth)
    expect(t.startsWith(""), "empty prefix matches root");
    expect(t.countWordsWithPrefix("") == 0, "no words yet, count 0");
    t.insert("a");
    expect(t.countWordsWithPrefix("") == 1, "count via empty prefix");
    expect(!t.contains(""), "still does not contain empty word");
}

}  // namespace

int main() {
    try {
        test_insert_contains();
        test_duplicates_count_once();
        test_startswith();
        test_count_prefix();
        test_words_with_prefix_sorted();
        test_empty_string_edge();
        std::cout << "All trie tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
