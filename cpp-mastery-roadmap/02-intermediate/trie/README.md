# Trie (Prefix Tree)

Lowercase prefix tree with word counts, prefix counting and prefix
enumeration in lexicographic order.

## Learning Goals

- Node structure: children[26] + endsHere + passes counters
- Why prefix queries are O(prefix length)
- Collecting all words under a subtree (DFS in char order)
- Duplicate handling: unique count vs occurrence count

## Non-Goals

- Unicode / uppercase keys
- Patricia / radix compression
- Erasing entries

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
