# Base64 Codec

RFC 4648 base64 encode/decode with strict validation (`=` placement,
alphabet checking) and a no-throw `isValid()`.

## Learning Goals

- Bit-grouping: 3 bytes -> 4 sextets, padding rules for remainders 1/2
- Index lookup table for encode, range checks for decode
- Why length must be a multiple of 4; where '=' may legally appear
- Roundtrip over all 256 byte values as a test oracle

## Non-Goals

- Base64url (URL-safe alphabet)
- Streaming chunked encoding
- MIME line wrapping (76-char lines)

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
