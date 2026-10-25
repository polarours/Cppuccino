// examples/pitfall-iterator-invalidation-pitfalls.cpp
// Demonstrates iterator invalidation pitfalls and the safe patterns.
// Compile with: g++ -std=c++17 -o pitfall_iterator_invalidation_pitfalls pitfall-iterator-invalidation-pitfalls.cpp

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

// --- 1. vector: insert may reallocate, every iterator dangles ------------

void vector_insert_invalidation() {
    std::cout << "--- vector: insert invalidates iterators ---\n";

    std::vector<int> values = {1, 2, 3, 4, 5};
    const std::size_t old_capacity = values.capacity();

    auto it = values.begin();

    // The insert may reallocate, which frees the old buffer.
    values.insert(values.begin(), 0);

    std::cout << "capacity before: " << old_capacity
              << ", after: " << values.capacity() << "\n";
    std::cout << "the iterator captured before the insert is now dangling\n";

    // BAD: dereferencing the invalidated iterator is undefined behaviour.
    // It reads memory that the reallocation already freed.
    //
    // std::cout << *it << "\n";
    (void)it;  // deliberately left undereferenced: reading it would be UB

    // GOOD: re-acquire the iterator from the container.
    std::cout << "good: " << *values.begin() << "\n";
}

// --- 2. vector: erase invalidates the erased element and everything after

void vector_erase_invalidation() {
    std::cout << "\n--- vector: erase invalidates iterators ---\n";

    std::vector<int> values = {1, 2, 3, 4, 5};
    auto it = values.begin();          // points at 1

    values.erase(values.begin());      // invalidates it

    // BAD: 'it' refers to the removed element.
    //
    // std::cout << *it << "\n";
    (void)it;  // deliberately left undereferenced: reading it would be UB

    // GOOD: erase returns the iterator to the next element.
    auto next = values.erase(values.begin());
    std::cout << "good: " << *next << " (erase returned the next iterator)\n";
}

// --- 3. map: erase invalidates the erased iterator ------------------------

void map_erase_invalidation() {
    std::cout << "\n--- map: erase invalidates the erased iterator ---\n";

    std::map<int, std::string> table = {{1, "one"}, {2, "two"}, {3, "three"}};

    auto it = table.begin();

    // BAD: 'it' is invalid after this call, but the code below still uses it.
    table.erase(it);
    // std::cout << it->second << "\n";   // undefined behaviour

    // GOOD: take the iterator that erase returns.
    it = table.begin();
    it = table.erase(it);
    std::cout << "good: " << it->first << " -> " << it->second << "\n";
}

// --- 4. Safe pattern: always take erase's return value -------------------

bool is_even(int value) {
    return value % 2 == 0;
}

void erase_by_return_value() {
    std::cout << "\n--- Safe: it = container.erase(it) ---\n";

    std::vector<int> values = {1, 2, 3, 4, 5, 6};

    for (auto it = values.begin(); it != values.end(); ) {
        if (is_even(*it)) {
            it = values.erase(it);  // returns the next valid iterator
        } else {
            ++it;
        }
    }

    std::cout << "remaining: ";
    for (int value : values) {
        std::cout << value << ' ';
    }
    std::cout << "\n";
}

// --- 5. Safe pattern: the erase-remove idiom -----------------------------

void erase_remove_idiom() {
    std::cout << "\n--- Safe: the erase-remove idiom ---\n";

    std::vector<int> values = {1, 2, 3, 4, 5, 6};

    values.erase(std::remove_if(values.begin(), values.end(), is_even), values.end());

    std::cout << "remaining: ";
    for (int value : values) {
        std::cout << value << ' ';
    }
    std::cout << "\n";
}

// --- 6. Safe pattern: index traversal for vector --------------------------

void index_traversal() {
    std::cout << "\n--- Safe: index traversal ---\n";

    std::vector<int> values = {1, 2, 3, 4, 5};

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (is_even(values[i])) {
            values[i] = 0;  // no size change, no iterator is invalidated
        }
    }

    std::cout << "after zeroing the even values: ";
    for (int value : values) {
        std::cout << value << ' ';
    }
    std::cout << "\n";
}

// --- 7. Containers whose iterators survive the modification ----------------

void stable_iterators() {
    std::cout << "\n--- list and set: insertion keeps iterators valid ---\n";

    std::list<int> chain = {2, 3};
    auto it = chain.begin();

    chain.push_front(1);  // list never reallocates its nodes
    chain.push_back(4);

    std::cout << "list iterator still valid, points at " << *it << "\n";

    std::set<int> keys = {2, 3};
    auto key_it = keys.begin();
    keys.insert(1);

    std::cout << "set iterator still valid, points at " << *key_it << "\n";
}

// --- 8. The rules, summarized ---------------------------------------------

void invalidation_rules() {
    std::cout << "\n--- Invalidation rules ---\n";
    std::cout << "vector, string : insert and erase invalidate every iterator\n";
    std::cout << "deque          : insert in the middle invalidates every iterator\n";
    std::cout << "list           : insertion keeps every iterator valid\n";
    std::cout << "map, set       : insert keeps iterators valid, erase returns the next\n";
    std::cout << "unordered_map  : insert may rehash and invalidate every iterator\n";
}

int main() {
    vector_insert_invalidation();
    vector_erase_invalidation();
    map_erase_invalidation();
    erase_by_return_value();
    erase_remove_idiom();
    index_traversal();
    stable_iterators();
    invalidation_rules();

    std::cout << "\n=== All iterator invalidation scenarios completed ===\n";
    return 0;
}
