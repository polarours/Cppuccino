// examples/pitfall-iterator-invalidation.cpp
// Demonstrates iterator invalidation pitfalls and their fixes.
// Compile with: g++ -std=c++17 -o pitfall_iterator_invalidation pitfall-iterator-invalidation.cpp

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// --- 1. Reallocation invalidates every iterator, reference and pointer ---

void reallocationInvalidatesHandles() {
    std::cout << "--- Reallocation invalidates handles ---\n";

    std::vector<int> vec = {1, 2, 3};
    auto it = vec.begin();

    // BAD: push_back may reallocate, which invalidates 'it', and reading through
    // an invalidated iterator is undefined behaviour.
    // vec.push_back(4);
    // std::cout << *it << "\n";

    // GOOD: reacquire the handle after the mutation.
    vec.push_back(4);
    it = vec.begin();
    std::cout << "Good: after push_back the reacquired iterator points at " << *it << "\n";
}

// --- 2. A cached end iterator becomes stale in a mutating loop ---

void cachedEndIterator() {
    std::cout << "\n--- Cached end iterator ---\n";

    std::vector<int> vec = {1, 2, 3};

    // BAD: 'end' is captured before the loop body grows the vector, so the
    // comparison against a stale end is undefined behaviour. The push_back
    // invalidates 'it' as well.
    // for (auto it = vec.begin(), e = vec.end(); it != e; ++it) {
    //     vec.push_back(*it);
    // }

    // GOOD: separate the read phase from the write phase.
    std::vector<int> doubled;
    for (int n : vec) {
        doubled.push_back(n * 2);
    }
    vec = doubled;

    std::cout << "Good: after the two-phase update the vector holds ";
    for (int n : vec) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 3. Erasing inside a loop invalidates the iterator ---

void eraseInsideLoop() {
    std::cout << "\n--- Erasing inside a loop ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};

    // BAD: erase invalidates 'it', and the ++it afterwards is undefined
    // behaviour.
    // for (auto it = vec.begin(); it != vec.end(); ++it) {
    //     if (*it % 2 == 0) {
    //         vec.erase(it);
    //     }
    // }

    // GOOD: use the iterator returned by erase.
    for (auto it = vec.begin(); it != vec.end(); ) {
        if (*it % 2 == 0) {
            it = vec.erase(it);
        } else {
            ++it;
        }
    }

    std::cout << "Good: only odd values remain: ";
    for (int n : vec) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 4. A pointer to an element does not survive growth ---

void pointerAcrossGrowth() {
    std::cout << "\n--- Pointer across growth ---\n";

    std::vector<int> vec = {10, 20, 30};
    int* p = &vec[0];

    // BAD: the growth may reallocate, so 'p' dangles and *p is undefined
    // behaviour.
    // vec.push_back(40);
    // std::cout << *p << "\n";

    // GOOD: reacquire the pointer, or address the element by index.
    vec.push_back(40);
    p = &vec[0];
    std::cout << "Good: reacquired pointer reads " << *p << ", element 3 is "
              << vec[3] << "\n";
}

// --- 5. Node based containers keep their iterators stable ---

void stableIterators() {
    std::cout << "\n--- Node based containers keep iterators stable ---\n";

    std::list<int> lst = {1, 2, 3, 4, 5};
    std::map<int, std::string> mp = {{1, "one"}, {2, "two"}, {3, "three"}};

    auto list_it = lst.begin();
    auto map_it = mp.begin();

    lst.insert(lst.begin(), 0);  // inserting does not invalidate the iterators
    mp.emplace(4, "four");

    // GOOD: the iterators still point at the same elements.
    std::cout << "Good: list iterator still reads " << *list_it
              << ", map iterator still reads " << map_it->first << "\n";
}

// --- 6. Rehashing an unordered container invalidates its iterators ---

void rehashInvalidatesIterators() {
    std::cout << "\n--- Rehashing invalidates iterators ---\n";

    std::unordered_map<int, std::string> mp;
    mp.emplace(1, "one");

    // BAD: inserting may rehash, which invalidates the iterator.
    // auto it = mp.begin();
    // mp.emplace(2, "two");
    // std::cout << it->first << "\n";

    // GOOD: reserve the bucket count up front, and reacquire after insertion.
    mp.reserve(16);
    auto it = mp.begin();
    mp.emplace(2, "two");
    it = mp.find(1);
    std::cout << "Good: after insertion the reacquired iterator reads " << it->first
              << "\n";
}

// --- 7. Fix: the erase-remove idiom ---

void eraseRemoveIdiom() {
    std::cout << "\n--- Fix: erase-remove idiom ---\n";

    std::vector<int> vec = {1, 2, 3, 2, 5, 2};
    vec.erase(std::remove(vec.begin(), vec.end(), 2), vec.end());

    std::cout << "Good: after removing every 2: ";
    for (int n : vec) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

// --- 8. Mutating a container while a range-based for walks it ---

void mutateDuringRangeFor() {
    std::cout << "\n--- Mutating during a range-based for ---\n";

    std::vector<int> vec = {1, 2, 3};

    // BAD: growing the vector inside the loop invalidates the iterator that
    // the range-based for is using.
    // for (auto& x : vec) {
    //     if (x == 2) {
    //         vec.push_back(99);
    //     }
    // }

    // GOOD: collect first, mutate afterwards.
    std::vector<int> to_add;
    for (int n : vec) {
        if (n == 2) {
            to_add.push_back(99);
        }
    }
    vec.insert(vec.end(), to_add.begin(), to_add.end());

    std::cout << "Good: after the two-phase update the vector holds ";
    for (int n : vec) {
        std::cout << n << " ";
    }
    std::cout << "\n";
}

int main() {
    reallocationInvalidatesHandles();
    cachedEndIterator();
    eraseInsideLoop();
    pointerAcrossGrowth();
    stableIterators();
    rehashInvalidatesIterators();
    eraseRemoveIdiom();
    mutateDuringRangeFor();

    std::cout << "\n=== All iterator invalidation scenarios completed ===\n";
    return 0;
}
