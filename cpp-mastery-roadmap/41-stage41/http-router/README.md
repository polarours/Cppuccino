# HTTP Router

Method + path routing with `:param` segments, extracted into a parameter
map handed to the handler.

## Learning Goals

- Pattern matching by segment count and literal/param rules
- Route dispatch returning 404 when nothing matches
- Extracting named path parameters without regex

## Non-Goals

- Middleware pipeline
- Query strings and headers
- Production routing edge cases

## Thread Safety

The route table is read and written under one mutex, so a route may be
added while another thread is matching a request. The handler itself is
called with the mutex released: a handler that wants to register
another route would deadlock on a mutex it already holds.

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
