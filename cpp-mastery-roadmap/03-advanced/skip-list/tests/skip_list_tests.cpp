#include "skip_list.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_insert_sorted_output() {
    skip_list::SkipList<int> sl;
    for (int v : {5, 1, 9, 3, 7}) sl.insert(v);
    auto v = sl.toVector();
    expect((v == std::vector<int>{1, 3, 5, 7, 9}), "ascending order");
    expect(sl.size() == 5, "size 5");
}

void test_duplicates_rejected() {
    skip_list::SkipList<int> sl;
    expect(sl.insert(4), "first insert true");
    expect(!sl.insert(4), "duplicate rejected");
    expect(sl.size() == 1, "size stays 1");
    expect(sl.contains(4), "still contains");
}

void test_contains() {
    skip_list::SkipList<int> sl;
    for (int i = 0; i < 100; i += 10) sl.insert(i);
    expect(sl.contains(0), "0 present");
    expect(sl.contains(90), "90 present");
    expect(!sl.contains(5), "5 absent");
    expect(!sl.contains(101), "101 absent");
}

void test_successor() {
    skip_list::SkipList<int> sl;
    for (int v : {10, 20, 30}) sl.insert(v);
    expect(sl.successor(10).value_or(-1) == 20, "10 -> 20");
    expect(sl.successor(20).value_or(-1) == 30, "20 -> 30");
    expect(!sl.successor(30).has_value(), "30 -> none");
    expect(sl.successor(15).value_or(-1) == 20, "between -> next");
    expect(!sl.successor(99).has_value(), "past end -> none");
}

void test_string_keys() {
    skip_list::SkipList<std::string> sl;
    for (auto* s : {"pear", "apple", "fig"}) sl.insert(s);
    expect((sl.toVector() == std::vector<std::string>{"apple", "fig", "pear"}),
           "string ordering");
}

void test_large_random_set_stays_sorted() {
    skip_list::SkipList<int> sl;
    std::vector<int> keys;
    for (int i = 0; i < 500; ++i) keys.push_back((i * 37 + 11) % 500);
    for (int k : keys) sl.insert(k);
    auto v = sl.toVector();
    expect(v.size() == 500, "all unique keys kept");
    expect(std::is_sorted(v.begin(), v.end()), "output sorted");
    expect(sl.contains(499), "spot check contains");
}

void test_empty() {
    skip_list::SkipList<int> sl;
    expect(sl.empty(), "starts empty");
    expect(!sl.contains(1), "empty contains nothing");
    expect(!sl.successor(1).has_value(), "no successor");
}

}  // namespace

int main() {
    try {
        test_insert_sorted_output();
        test_duplicates_rejected();
        test_contains();
        test_successor();
        test_string_keys();
        test_large_random_set_stays_sorted();
        test_empty();
        std::cout << "All skip_list tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
