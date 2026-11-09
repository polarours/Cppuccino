# JSON Parser

A simple JSON parser and serializer in modern C++.

## Learning Goals

- Understand recursive descent parsing
- Practice std::variant and tagged unions
- Learn JSON format specification
- Implement a state machine parser

A number that is cut short is not a number: "1e", "1." and "01" are
refused rather than read as something close to them.

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

The serializer escapes the characters that need it as well, so a value
that was parsed can be written out and parsed again. A whole number is
written without a decimal point, as long as the double holds it
exactly.

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
