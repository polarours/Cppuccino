#include "binary_heap.hpp"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_max_heap_order() {
    binary_heap::BinaryHeap<int> h;
    for (int v : {5, 3, 8, 1, 9}) h.push(v);
    expect(h.size() == 5, "size 5");
    std::vector<int> out;
    while (!h.empty()) out.push_back(h.pop());
    expect((out == std::vector<int>{9, 8, 5, 3, 1}), "descending order");
}

void test_min_heap_order() {
    binary_heap::BinaryHeap<int, std::greater<int>> h;
    for (int v : {5, 3, 8, 1, 9}) h.push(v);
    std::vector<int> out;
    while (!h.empty()) out.push_back(h.pop());
    expect((out == std::vector<int>{1, 3, 5, 8, 9}), "ascending order");
}

void test_empty_throws() {
    binary_heap::BinaryHeap<int> h;
    bool threwPop = false, threwTop = false;
    try { h.pop(); } catch (const std::runtime_error&) { threwPop = true; }
    try { (void)h.top(); } catch (const std::runtime_error&) { threwTop = true; }
    expect(threwPop, "pop on empty throws");
    expect(threwTop, "top on empty throws");
}

void test_top_unchanged_after_push_smaller() {
    binary_heap::BinaryHeap<int> h;
    h.push(10);
    h.push(2);
    expect(h.top() == 10, "top stays 10");
    h.push(20);
    expect(h.top() == 20, "top rises to 20");
}

void test_heapify_faster_path() {
    binary_heap::BinaryHeap<int> h;
    h.heapify({2, 7, 1, 9, 4, 4, 6});
    // heapify must preserve multiset and heap invariant
    expect(h.size() == 7, "size after heapify");
    std::vector<int> out;
    while (!h.empty()) out.push_back(h.pop());
    expect((out == std::vector<int>{9, 7, 6, 4, 4, 2, 1}), "heapify pop order");
}

void test_single_element() {
    binary_heap::BinaryHeap<std::string> h;
    h.push("only");
    expect(h.top() == "only", "top");
    expect(h.pop() == "only", "pop");
    expect(h.empty(), "empty after pop");
}

}  // namespace

int main() {
    try {
        test_max_heap_order();
        test_min_heap_order();
        test_empty_throws();
        test_top_unchanged_after_push_smaller();
        test_heapify_faster_path();
        test_single_element();
        std::cout << "All binary_heap tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
