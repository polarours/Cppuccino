# JSON Parser

A simple JSON parser and serializer in modern C++.

## Learning Goals

- Understand recursive descent parsing
- Practice std::variant and tagged unions
- Learn JSON format specification
- Implement a state machine parser

## Non-Goals

- Building a production-grade JSON parser
- Supporting full JSON specification (comments, trailing commas)
- Implementing JSON path or query language

## Features

- Parse JSON strings into a value tree
- Serialize values back to JSON strings
- Support for null, boolean, number, string, array, object
- Pretty-print with indentation
- Escape sequence handling in strings

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/json_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "json.hpp"

// Parse JSON
auto value = json::JsonValue::parse(R"({"name": "test", "value": 42})");

// Access values
std::string name = value.asObject().at("name").asString();
double val = value.asObject().at("value").asNumber();

// Build JSON
json::JsonObject obj;
obj["key"] = json::JsonValue("value");
json::JsonValue json(obj);

// Serialize
std::string output = json.stringify(2);  // Pretty print
```

## Suggested Next Steps

- Add support for JSON comments
- Implement JSON path queries
- Add schema validation
- Support streaming parsing
