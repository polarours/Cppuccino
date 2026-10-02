#include "trie.hpp"

namespace trie {

namespace {
inline int idx(char c) { return c - 'a'; }
inline bool valid(char c) { return c >= 'a' && c <= 'z'; }
}  // namespace

void Trie::insert(const std::string& word) {
    Node* cur = root_.get();
    // Root passes = total insertions (so countWordsWithPrefix("") works)
    ++root_->passes;
    for (char c : word) {
        if (!valid(c)) return;  // lowercase-only; ignore invalid input
        int i = idx(c);
        if (!cur->children[i]) cur->children[i] = std::make_unique<Node>();
        cur = cur->children[i].get();
        ++cur->passes;
    }
    if (cur->endsHere == 0) ++wordCount_;
    ++cur->endsHere;
}

Trie::Node* Trie::descend(const std::string& s) const {
    Node* cur = root_.get();
    for (char c : s) {
        if (!valid(c)) return nullptr;
        cur = cur->children[idx(c)].get();
        if (!cur) return nullptr;
    }
    return cur;
}

bool Trie::contains(const std::string& word) const {
    Node* n = descend(word);
    return n && n->endsHere > 0;
}

bool Trie::startsWith(const std::string& prefix) const {
    return descend(prefix) != nullptr;
}

int Trie::countWordsWithPrefix(const std::string& prefix) const {
    Node* n = descend(prefix);
    return n ? n->passes : 0;
}

void Trie::collect(const Node* node, const std::string& prefix,
                   std::vector<std::string>& out) const {
    if (!node) return;
    for (int i = 0; i < node->endsHere; ++i) out.push_back(prefix);
    for (int c = 0; c < 26; ++c) {
        if (node->children[c])
            collect(node->children[c].get(), prefix + char('a' + c), out);
    }
}

std::vector<std::string> Trie::wordsWithPrefix(const std::string& prefix) const {
    Node* n = descend(prefix);
    std::vector<std::string> out;
    collect(n, prefix, out);
    return out;
}

}  // namespace trie
