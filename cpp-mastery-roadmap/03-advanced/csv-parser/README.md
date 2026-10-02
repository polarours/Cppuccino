# CSV Parser

RFC-4180-style CSV: quoted fields with commas/newlines/doubled quotes,
CRLF tolerance, header row, and format() for lossless round-trips.

## Learning Goals

- State machine for quoting rules (in-quote vs out-of-quote)
- Why doubled quotes ("") exist and how to unescape them
- Row-to-column alignment for short rows
- Format/parse roundtrip as a correctness oracle

## Non-Goals

- Schema validation / type inference
- Streaming from files (input is a string)
- Delimiters other than comma

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
