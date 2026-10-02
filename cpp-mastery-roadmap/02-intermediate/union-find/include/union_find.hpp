#pragma once

#include <cstddef>
#include <numeric>
#include <vector>

namespace union_find {

// Disjoint Set Union with path compression + union by rank.
class DSU {
public:
    explicit DSU(int n) : parent_(n), rank_(n, 0), components_(n) {
        std::iota(parent_.begin(), parent_.end(), 0);
    }

    int find(int x) {
        // Iterative path compression (no recursion depth risk)
        int root = x;
        while (parent_[root] != root) root = parent_[root];
        while (parent_[x] != root) {
            int next = parent_[x];
            parent_[x] = root;
            x = next;
        }
        return root;
    }

    // Returns true when a and b were in different sets (merged now).
    bool unite(int a, int b) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return false;
        if (rank_[ra] < rank_[rb]) std::swap(ra, rb);
        parent_[rb] = ra;
        if (rank_[ra] == rank_[rb]) ++rank_[ra];
        --components_;
        return true;
    }

    bool connected(int a, int b) { return find(a) == find(b); }
    int componentCount() const { return components_; }
    int size() const { return static_cast<int>(parent_.size()); }

private:
    std::vector<int> parent_;
    std::vector<int> rank_;
    int components_;
};

}  // namespace union_find
