# Text Diff (LCS)

Line-based diff using the classic longest-common-subsequence algorithm,
with rendering and a similarity score.

## Learning Goals

- LCS dynamic programming (bottom-up backtrack to an edit script)
- Edit script semantics: Equal / Insert / Delete ordering
- Similarity metric derived from the diff
- Render diffs in unified-diff style

## Non-Goals

- Myers' O(ND) algorithm
- Word-level or char-level diffs
- Hunk grouping with context lines

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/text_diff_example
```

## Test

```bash
ctest --test-dir build --output-on-failure
```
