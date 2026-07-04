// examples/pitfall-iterator-invalidation.cpp
// Demonstrates iterator invalidation pitfalls and their fixes.
// Compile with: g++ -std=c++17 -o pitfall_iterator pitfall-iterator-invalidation.cpp

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>

// --- 1. Vector Iterator Invalidation ---

void vectorIteratorInvalidation() {
    std::cout << "--- Vector Iterator Invalidation ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};

    // BAD: Erasing while iterating
    // for (auto it = vec.begin(); it != vec.end(); ++it) {
    //     if (*it == 3) {
    //         vec.erase(it);  // it is now invalid!
    //     }
    // }

    // GOOD: Use erase return value
    for (auto it = vec.begin(); it != vec.end(); ) {
        if (*it == 3) {
            it = vec.erase(it);  // erase returns next valid iterator
        } else {
            ++it;
        }
    }

    std::cout << "After erase: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 2. Vector Insert Invalidation ---

void vectorInsertInvalidation() {
    std::cout << "\n--- Vector Insert Invalidation ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};

    // BAD: Insert may invalidate all iterators
    // auto it = vec.begin() + 2;
    // vec.push_back(6);  // it may be invalid now
    // std::cout << *it << "\n";  // Undefined behavior

    // GOOD: Revalidate iterator after modification
    auto it = vec.begin() + 2;
    vec.push_back(6);
    it = vec.begin() + 2;  // Revalidate
    std::cout << "After push_back: " << *it << "\n";
}

// --- 3. List Iterator Invalidation ---

void listIteratorInvalidation() {
    std::cout << "\n--- List Iterator Invalidation ---\n";

    std::list<int> lst = {1, 2, 3, 4, 5};

    // List iterators are NOT invalidated by insert/erase at other positions
    for (auto it = lst.begin(); it != lst.end(); ) {
        if (*it == 3) {
            it = lst.erase(it);  // Other iterators remain valid
        } else {
            ++it;
        }
    }

    std::cout << "After erase: ";
    for (int n : lst) std::cout << n << " ";
    std::cout << "\n";
}

// --- 4. Map Iterator Invalidation ---

void mapIteratorInvalidation() {
    std::cout << "\n--- Map Iterator Invalidation ---\n";

    std::map<int, std::string> mp = {
        {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}
    };

    // Map iterators are NOT invalidated by insert/erase at other positions
    for (auto it = mp.begin(); it != mp.end(); ) {
        if (it->first == 3) {
            it = mp.erase(it);
        } else {
            ++it;
        }
    }

    std::cout << "After erase: ";
    for (const auto& [k, v] : mp) {
        std::cout << k << ":" << v << " ";
    }
    std::cout << "\n";
}

// --- 5. Set Iterator Invalidation ---

void setIteratorInvalidation() {
    std::cout << "\n--- Set Iterator Invalidation ---\n";

    std::set<int> s = {1, 2, 3, 4, 5};

    for (auto it = s.begin(); it != s.end(); ) {
        if (*it == 3) {
            it = s.erase(it);
        } else {
            ++it;
        }
    }

    std::cout << "After erase: ";
    for (int n : s) std::cout << n << " ";
    std::cout << "\n";
}

// --- 6. Fix: Erase-Remove Idiom ---

void eraseRemoveIdiom() {
    std::cout << "\n--- Fix: Erase-Remove Idiom ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5, 3, 6, 3};

    // Remove all 3s
    vec.erase(std::remove(vec.begin(), vec.end(), 3), vec.end());

    std::cout << "After remove 3: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 7. Fix: Remove If ---

void fixRemoveIf() {
    std::cout << "\n--- Fix: Remove If ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8};

    // Remove even numbers
    vec.erase(
        std::remove_if(vec.begin(), vec.end(), [](int n) { return n % 2 == 0; }),
        vec.end()
    );

    std::cout << "After remove even: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 8. Fix: Collect First, Then Remove ---

void fixCollectThenRemove() {
    std::cout << "\n--- Fix: Collect First, Then Remove ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<int> toRemove;

    // Collect items to remove
    for (int n : vec) {
        if (n % 2 == 0) {
            toRemove.push_back(n);
        }
    }

    // Remove collected items
    for (int n : toRemove) {
        vec.erase(std::remove(vec.begin(), vec.end(), n), vec.end());
    }

    std::cout << "After removing evens: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 9. Fix: Use Index Instead of Iterator ---

void fixUseIndex() {
    std::cout << "\n--- Fix: Use Index Instead of Iterator ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < vec.size(); ) {
        if (vec[i] == 3) {
            vec.erase(vec.begin() + i);
            // Don't increment i
        } else {
            ++i;
        }
    }

    std::cout << "After erase by index: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 10. Fix: Copy and Modify ---

void fixCopyAndModify() {
    std::cout << "\n--- Fix: Copy and Modify ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<int> result;

    std::copy_if(vec.begin(), vec.end(), std::back_inserter(result),
        [](int n) { return n != 3; });

    std::cout << "After copy_if: ";
    for (int n : result) std::cout << n << " ";
    std::cout << "\n";
}

int main() {
    vectorIteratorInvalidation();
    vectorInsertInvalidation();
    listIteratorInvalidation();
    mapIteratorInvalidation();
    setIteratorInvalidation();
    eraseRemoveIdiom();
    fixRemoveIf();
    fixCollectThenRemove();
    fixUseIndex();
    fixCopyAndModify();

    std::cout << "\n=== All iterator invalidation scenarios completed ===\n";
    return 0;
}
