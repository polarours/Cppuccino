# Markdown Parser

A minimal markdown-to-blocks parser: headings, list items, quotes, fenced
code blocks, inline links and formatting.

## Learning Goals

- Parse line-oriented text formats with a state machine (in-code fence state)
- Keep code fences verbatim so inner syntax is not reinterpreted
- Extract inline links with a small scanner
- Strip inline markers without touching surrounding text

## Non-Goals

- Full CommonMark compliance
- HTML output
- Nested block structures

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/markdown_parser_example
```

## Test

```bash
ctest --test-dir build --output-on-failure
```
