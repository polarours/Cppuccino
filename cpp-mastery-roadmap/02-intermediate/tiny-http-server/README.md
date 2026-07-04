# tiny-http-server

A small HTTP server written in modern C++.

This project is part of the **Cppuccino** learning roadmap and is intended to
show how socket handling, request parsing, routing, and response construction
fit together in a minimal server.

## Learning Goals

- understand how socket resources move through a small server
- see how request parsing, routing, and response construction should stay separated
- practice explicit ownership and lifetime boundaries across modules
- learn a simple fixed-size thread-pool model without introducing too much machinery
- read code that is small enough to refactor but large enough to have real structure

## Non-Goals

- implementing a complete HTTP server
- covering HTTP/1.1 keep-alive, chunked transfer, or TLS
- building a production-grade concurrency model
- optimizing throughput before the boundaries are stable

## Current Features

- TCP socket server on IPv4
- Fixed-size thread pool for handling accepted connections
- Minimal HTTP request-line and header parsing
- Simple router for `GET /` and `GET /hello`
- HTTP response builder with status line, headers, and body
- Basic error responses:
  `400 Bad Request`, `404 Not Found`, and `405 Method Not Allowed`

## Current Limits

- Reads a request in a single pass with a fixed buffer
- Uses a simple one-task-per-connection model
- Supports only a small set of hard-coded routes
- Does not yet support graceful shutdown or keep-alive connections

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/tiny_http_server
```

The server listens on `0.0.0.0:8082`.
The current `main` function starts it with `4` worker threads.

## Test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Try It

```bash
curl http://127.0.0.1:8082/
curl http://127.0.0.1:8082/hello
curl http://127.0.0.1:8082/missing
curl -X POST http://127.0.0.1:8082/hello
```

## Suggested Next Steps

- Replace hard-coded routes with handler registration
- Add RAII wrappers for file descriptors
- Improve request parsing and partial-read handling
- Add tests for server-level behavior and shutdown paths
