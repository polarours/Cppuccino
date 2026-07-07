# HTTP Parser

A simple HTTP request/response parser.

## Learning Goals

- Understand text protocol parsing
- Practice string manipulation and state machines
- Learn HTTP/1.1 request and response formats
- Implement header parsing with case-insensitive lookup

## Non-Goals

- Building a complete HTTP server
- Supporting HTTP/2 or chunked encoding
- Implementing keep-alive connections

## Features

- Parse HTTP requests (GET, POST, PUT, DELETE, HEAD)
- Parse request headers and body
- Build HTTP responses with status codes
- Case-insensitive header lookup
- Response serialization

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/http_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "http.hpp"

// Parse request
http::Request req;
req.parse("GET /api/data HTTP/1.1\r\nHost: example.com\r\n\r\n");
std::cout << req.methodName() << " " << req.path << "\n";

// Build response
auto resp = http::Response::ok("Hello, World!");
std::cout << resp.serialize();
```

## Suggested Next Steps

- Add chunked transfer encoding support
- Implement request body parsing (form data, multipart)
- Add HTTP/2 frame parsing
- Implement a simple HTTP server using this parser
