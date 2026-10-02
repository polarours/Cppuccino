# Link Checker

Validates relative markdown links against a known set of files, with
path normalization and optional anchor verification.

## Learning Goals

- Lexical path normalization (`./`, `../`) without touching the filesystem
- Skipping code spans and external links during extraction
- Anchor tracking: report missing `#anchors` only when the caller tracks them
- Turn a common docs chore into a testable unit

## Non-Goals

- Fetching HTTP links
- Walking a real filesystem (the caller supplies the file set)
- Full markdown parsing

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/link_checker_example
```

## Test

```bash
ctest --test-dir build --output-on-failure
```
