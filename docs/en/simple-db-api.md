# Simple Database API

## Overview

`simple-db` is an in-memory database that supports table management, CRUD
operations and file persistence. Its core value type is built on
`std::variant`, so a cell can hold more than one type.

## Core Types

### Value

```cpp
using Value = std::variant<int, double, std::string, bool, std::nullptr_t>;
```

Five types are supported: integer, floating point, string, boolean and null.

### Row / Rows

```cpp
using Row = std::map<std::string, Value>;
using Rows = std::vector<Row>;
```

- `Row`: one row of data; keys are column names, values are cell values.
- `Rows`: several rows of data.

## Database Class API

### Construction and Table Management

| Method | Signature | Description |
|---|---|---|
| `createTable` | `bool createTable(const std::string& name)` | Creates a table; returns false if it already exists |
| `dropTable` | `bool dropTable(const std::string& name)` | Drops a table; returns false if it does not exist |
| `tableExists` | `bool tableExists(const std::string& name) const` | Reports whether a table exists |

### CRUD Operations

| Method | Signature | Description |
|---|---|---|
| `insert` | `bool insert(const std::string& table, const Row& row)` | Inserts a row; returns false if the table does not exist |
| `select` | `Rows select(const std::string& table, const std::string& where = "", const std::function<bool(const Row&)>& predicate = nullptr)` | Returns matching rows, filtered by an optional predicate |
| `update` | `bool update(const std::string& table, const std::map<std::string, Value>& updates, const std::function<bool(const Row&)>& predicate = nullptr)` | Updates matching rows; returns whether anything changed |
| `remove` | `bool remove(const std::string& table, const std::function<bool(const Row&)>& predicate = nullptr)` | Removes matching rows; returns whether any row was removed |

The `where` parameter of `select` is accepted but not used: filtering happens
through `predicate` only.

### Utility Methods

| Method | Signature | Description |
|---|---|---|
| `count` | `std::size_t count(const std::string& table) const` | Returns the number of rows in a table |
| `clear` | `void clear(const std::string& table)` | Empties a table but keeps its structure |
| `tables` | `const std::map<std::string, Rows>& tables() const` | Returns every table with its data |

### Persistence

| Method | Signature | Description |
|---|---|---|
| `save` | `bool save(const std::string& filename) const` | Writes the database to a file; returns false if the file cannot be opened |
| `load` | `bool load(const std::string& filename)` | Reads the database from a file, replacing the current contents; returns false if the file cannot be opened |

The file format is line based: a table header line `TABLE:<name>`, followed by
one `ROW|key=value|key=value` line per row.

## Helper Functions

```cpp
std::string valueToString(const Value& value);
Value stringToValue(const std::string& str);
```

These serialize a `Value` to and from a string for file storage.

## Complete Example

```cpp
#include "simple_db.hpp"
#include <iostream>

int main() {
    simple_db::Database db;

    // Create a table.
    db.createTable("users");

    // Insert rows.
    db.insert("users", {{"name", "Alice"}, {"age", 30}});
    db.insert("users", {{"name", "Bob"}, {"age", 25}});
    db.insert("users", {{"name", "Charlie"}, {"age", 35}});

    // Query everything.
    auto all = db.select("users");
    std::cout << "Total users: " << all.size() << "\n";

    // Filter with a predicate.
    auto adults = db.select("users", "", [](const simple_db::Row& row) {
        auto it = row.find("age");
        if (it != row.end() && std::holds_alternative<int>(it->second)) {
            return std::get<int>(it->second) >= 18;
        }
        return false;
    });
    std::cout << "Adults: " << adults.size() << "\n";

    // Update the matching rows.
    db.update("users", {{"age", 31}}, [](const simple_db::Row& row) {
        auto it = row.find("name");
        return it != row.end() && std::get<std::string>(it->second) == "Alice";
    });

    // Remove the matching rows.
    db.remove("users", [](const simple_db::Row& row) {
        auto it = row.find("name");
        return it != row.end() && std::get<std::string>(it->second) == "Bob";
    });

    // Persist to disk.
    db.save("database.txt");

    return 0;
}
```

## Build and Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
