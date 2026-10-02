#include "trie.hpp"

#include <iostream>

int main() {
    trie::Trie t;
    for (auto* w : {"car", "card", "care", "dog", "do"})
        t.insert(w);

    std::cout << std::boolalpha;
    std::cout << "contains 'car': " << t.contains("car") << "\n";
    std::cout << "contains 'ca': " << t.contains("ca") << "\n";
    std::cout << "startsWith 'ca': " << t.startsWith("ca") << "\n";
    std::cout << "words with 'ca':";
    for (auto& w : t.wordsWithPrefix("ca")) std::cout << " " << w;
    std::cout << "\nunique words: " << t.size() << "\n";
    return 0;
}
