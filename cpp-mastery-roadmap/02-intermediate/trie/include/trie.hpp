#pragma once

#include <memory>
#include <string>
#include <vector>

namespace trie {

// Prefix tree over lowercase keys with word counts (supports duplicates).
class Trie {
public:
    Trie() : root_(std::make_unique<Node>()) {}

    void insert(const std::string& word);
    bool contains(const std::string& word) const;
    bool startsWith(const std::string& prefix) const;
    int countWordsWithPrefix(const std::string& prefix) const;  // exact prefix occurrences
    // All stored words beginning with prefix (lexicographic order).
    std::vector<std::string> wordsWithPrefix(const std::string& prefix) const;
    std::size_t size() const { return wordCount_; }

private:
    struct Node {
        std::unique_ptr<Node> children[26];
        int endsHere = 0;   // words finishing at this node
        int passes = 0;     // words passing through (incl. finishing here)
    };

    Node* descend(const std::string& s) const;  // nullptr when missing
    void collect(const Node* node, const std::string& prefix,
                 std::vector<std::string>& out) const;

    std::unique_ptr<Node> root_;
    std::size_t wordCount_ = 0;
};

}  // namespace trie
