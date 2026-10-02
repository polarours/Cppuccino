#pragma once

#include <memory>
#include <optional>
#include <random>
#include <vector>

namespace skip_list {

// Probabilistic ordered set (sorted set semantics: duplicates ignored).
// Nodes carry forward pointers; levels chosen with p = 1/2 coin flips.
template <typename T>
class SkipList {
public:
    explicit SkipList(std::size_t maxHeight = 16, std::uint32_t seed = 42)
        : maxHeight_(maxHeight), rng_(seed) {
        head_ = std::make_unique<Node>(T{}, maxHeight_);
    }

    bool insert(const T& value) {
        std::vector<Node*> predecessors(maxHeight_);
        findPredecessors(value, predecessors);

        // Already present? (set semantics)
        Node* nxt = predecessors[0]->forward[0];
        if (nxt && !(value < nxt->value) && !(nxt->value < value)) return false;

        int level = randomLevel();
        Node* node = new Node(value, level);
        for (int i = 0; i <= level; ++i) {
            node->forward[i] = predecessors[i]->forward[i];
            predecessors[i]->forward[i] = node;
        }
        ++size_;
        return true;
    }

    bool contains(const T& value) const {
        std::vector<Node*> preds(maxHeight_);
        findPredecessors(value, preds);
        Node* nxt = preds[0]->forward[0];
        return nxt && !(value < nxt->value) && !(nxt->value < value);
    }

    std::optional<T> successor(const T& value) const {
        std::vector<Node*> preds(maxHeight_);
        findPredecessors(value, preds);
        Node* nxt = preds[0]->forward[0];
        if (!nxt) return std::nullopt;
        // if equal to value, step to the actual successor
        if (!(value < nxt->value) && !(nxt->value < value)) nxt = nxt->forward[0];
        if (nxt == nullptr) return std::nullopt;
        return nxt->value;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // Ascending traversal (for tests / debugging)
    std::vector<T> toVector() const {
        std::vector<T> out;
        for (Node* n = head_->forward[0]; n; n = n->forward[0]) out.push_back(n->value);
        return out;
    }

private:
    struct Node {
        T value;
        std::vector<Node*> forward;
        explicit Node(const T& v, int level) : value(v), forward(level + 1, nullptr) {}
    };

    // p = 1/2: keep climbing while coin is heads, capped at maxHeight_-1
    int randomLevel() {
        int level = 0;
        while ((rng_() & 1u) && level + 1 < static_cast<int>(maxHeight_)) ++level;
        return level;
    }

    template <typename V>
    void findPredecessors(const V& value, std::vector<Node*>& preds) const {
        Node* cur = head_.get();
        for (int lvl = static_cast<int>(maxHeight_) - 1; lvl >= 0; --lvl) {
            while (cur->forward[lvl] &&
                   cur->forward[lvl]->value < value) {
                cur = cur->forward[lvl];
            }
            preds[lvl] = cur;
        }
    }

    std::size_t maxHeight_;
    std::unique_ptr<Node> head_;
    std::size_t size_ = 0;
    mutable std::mt19937 rng_;
};

}  // namespace skip_list
