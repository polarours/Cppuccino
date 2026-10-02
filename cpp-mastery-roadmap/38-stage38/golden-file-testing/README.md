# Golden File Testing

Snapshot ("golden file") assertion helpers: line diffs on mismatch,
UPDATE_GOLDEN acceptance runs, and normalization rules for volatile
output (timestamps, ids).

## Learning Goals

- When snapshot tests beat example assertions (CLI output, serialized docs)
- Why "missing golden + UPDATE_GOLDEN=1" must be explicit (no silent bless)
- Normalization as the cure for nondeterministic fields
- Bounded diff reports for readable failures

## Non-Goals

- Binary/image comparison
- Review tooling (approval-tests UIs)
- Auto-update on CI

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
