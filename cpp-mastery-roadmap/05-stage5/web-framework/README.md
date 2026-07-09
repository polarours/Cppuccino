# Web Framework

A simple HTTP web framework with routing and middleware support.

## Learning Goals

- Understand HTTP protocol and request/response
- Practice socket programming and concurrency
- Learn routing and middleware patterns
- Implement static file serving

## Non-Goals

- Building a production web framework
- Supporting HTTP/2 or TLS
- Implementing complex middleware chains

## Features

- HTTP request parsing
- Route registration (GET, POST, PUT, DELETE)
- Middleware support
- Response builder with chaining
- Static file serving

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/web_framework_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "web_framework.hpp"

web_framework::App app;

app.get("/", [](const web_framework::Request&) {
    return web_framework::Response::html("<h1>Hello, World!</h1>");
});

app.get("/api/users", [](const web_framework::Request&) {
    return web_framework::Response::json(R"({"users": []})");
});

app.listen(8080);
```

## Suggested Next Steps

- Add URL parameter parsing (/users/:id)
- Implement JSON request/response
- Add WebSocket support
- Implement template engine
