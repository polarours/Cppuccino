# Template Engine

A minimal mustache-like engine: `{{var}}`, truthy sections `{{#k}}...{{/k}}`,
inverted sections `{{^k}}...{{/k}}`, and per-item list rendering.

## Learning Goals

- Tokenizing `{{tags}}` while keeping literal text intact
- Recursive section rendering with matched close tags
- Context chaining: item keys shadow parent context
- Falsy semantics for bool and empty list

## Non-Goals

- HTML escaping
- Partial templates / inheritance
- Whitespace control

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
