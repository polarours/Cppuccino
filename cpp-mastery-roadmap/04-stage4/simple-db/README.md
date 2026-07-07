# Simple Database

A simple in-memory database with table management and basic queries.

## Learning Goals

- Understand data modeling and storage
- Practice variant types for flexible values
- Learn file persistence patterns
- Implement CRUD operations

## Non-Goals

- Building a production database
- Implementing SQL parser
- Supporting concurrent access

## Features

- Create/drop tables
- Insert, select, update, delete rows
- Filter with predicates
- Save/load to file
- Multiple data types (int, double, string, bool, null)

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/simple_db_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "simple_db.hpp"

simple_db::Database db;
db.createTable("users");
db.insert("users", {{"name", "Alice"}, {"age", 25}});

auto users = db.select("users");
db.save("database.txt");
```

## Suggested Next Steps

- Add SQL-like query parser
- Implement indexing for faster lookups
- Add transaction support
- Implement WHERE clause parsing
