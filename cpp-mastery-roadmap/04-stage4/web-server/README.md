# Web Server

A simple HTTP web server with routing and middleware support.

## Learning Goals

- Understand HTTP protocol and request/response
- Practice socket programming and concurrency
- Learn routing and middleware patterns
- Implement static file serving

## Non-Goals

- Building a production web server
- Supporting HTTP/2 or TLS
- Implementing complex middleware chains

## Features

- HTTP request parsing
- Route registration (GET, POST, PUT, DELETE)
- Middleware support
- Static file serving
- Thread-based concurrency

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/web_server_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "web_server.hpp"

web_server::WebServer server(8080);

server.get("/", [](const web_server::Request&) {
    return web_server::Response::ok("Hello, World!");
});

server.get("/users/:id", [](const web_server::Request& req) {
    return web_server::Response::ok("User " + req.path);
});

server.start();
```

## Suggested Next Steps

- Add URL parameter parsing (/users/:id)
- Implement JSON request/response
- Add WebSocket support
- Implement connection pooling
