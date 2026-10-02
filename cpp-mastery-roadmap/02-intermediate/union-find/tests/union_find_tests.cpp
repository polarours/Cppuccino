#include "union_find.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_initial_state() {
    union_find::DSU d(5);
    expect(d.componentCount() == 5, "5 singleton components");
    expect(d.size() == 5, "size 5");
    for (int i = 0; i < 5; ++i)
        expect(d.find(i) == i, "singleton root is itself");
}

void test_unite_and_connect() {
    union_find::DSU d(6);
    expect(d.unite(0, 1), "first unite returns true");
    expect(d.unite(2, 3), "second unite returns true");
    expect(!d.unite(0, 1), "re-unite same set returns false");
    expect(d.connected(0, 1), "0-1 connected");
    expect(!d.connected(0, 2), "0-2 not connected");
    expect(d.componentCount() == 4, "4 components after 2 merges");
}

void test_transitive_closure() {
    union_find::DSU d(5);
    d.unite(0, 1);
    d.unite(1, 2);
    expect(d.connected(0, 2), "transitive: 0-1-2");
    expect(!d.connected(0, 3), "3 still separate");
}

void test_components_decrement_once() {
    union_find::DSU d(3);
    int merges = 0;
    if (d.unite(0, 1)) ++merges;
    if (d.unite(1, 0)) ++merges;  // already merged
    if (d.unite(0, 1)) ++merges;
    expect(merges == 1, "only first merge reports true");
    expect(d.componentCount() == 2, "component count decremented once");
}

void test_all_connected_one_component() {
    union_find::DSU d(4);
    for (int i = 1; i < 4; ++i) d.unite(0, i);
    expect(d.componentCount() == 1, "single component");
    for (int i = 1; i < 4; ++i)
        expect(d.connected(0, i), "all linked to 0");
}

void test_larger_chain_with_path_compression() {
    // Chain 0-1-...-999 then find from both ends
    union_find::DSU d(1000);
    for (int i = 0; i < 999; ++i) d.unite(i, i + 1);
    expect(d.componentCount() == 1, "one component");
    expect(d.connected(0, 999), "ends connected");
    // After compression, find should be near-flat
    int r = d.find(500);
    expect(d.find(0) == r && d.find(999) == r, "same root after compression");
}

}  // namespace

int main() {
    try {
        test_initial_state();
        test_unite_and_connect();
        test_transitive_closure();
        test_components_decrement_once();
        test_all_connected_one_component();
        test_larger_chain_with_path_compression();
        std::cout << "All union_find tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
