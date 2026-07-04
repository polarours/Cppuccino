# key-value-store

A small stateful storage exercise written in modern C++.

This project is part of the intermediate roadmap and is intended to teach state
ownership, command handling, and persistence boundaries in a much more direct
way than the HTTP server project.

## Learning Goals

- represent long-lived state explicitly
- keep command parsing separate from store behavior
- make persistence decisions visible instead of implicit
- define simple failure behavior for malformed commands and missing keys
- build a small system that is stateful enough to require tests

## Non-Goals

- building a production database
- supporting complex query languages or transactions
- solving concurrency or distributed consistency in v1
- hiding storage behavior behind over-general abstractions

## Current Features

- in-memory `std::unordered_map<std::string, std::string>` store
- thread-safe with read-write locks (shared_mutex)
- line-oriented command processor with explicit command actions
- commands:
  `SET`, `GET`, `DELETE`, `EXISTS`, `COUNT`, `CLEAR`, `SAVE`, `LOAD`, `EXIT`
- simple text persistence using quoted key/value pairs
- text protocol layer for structured request/response
- TCP server for network access with multi-client support
- CLI mode for local interaction
- native tests for store behavior, command handling, persistence, and protocol

## Protocol

The protocol layer provides a structured request/response format:

### Request Format

```
COMMAND [ARG1 [ARG2 ...]]
```

### Response Format

```
+OK\n                    (success)
+data\n                  (success with data)
$bulk_string\n           (value response)
:not_found\n             (key not found)
-ERROR message\n         (error)
:count\n                 (integer response)
```

### Protocol Commands

| Command | Description | Response |
|---------|-------------|----------|
| `SET key value` | Set a key-value pair | `+OK` |
| `GET key` | Get value by key | `$value` or `-NOT_FOUND` |
| `DEL key` | Delete a key | `+OK` or `-NOT_FOUND` |
| `EXISTS key` | Check if key exists | `+1` or `+0` |
| `COUNT` | Get number of entries | `:count` |
| `CLEAR` | Clear all entries | `+OK` |
| `PING` | Ping server | `+pong` |
| `HELP` | Show help | `$help_text` |

### Example Session

```
SET language C++
+OK
GET language
$C++
EXISTS language
+1
COUNT
:1
DEL language
+OK
GET language
-NOT_FOUND
PING
+pong
```

## Current Limits

- single-process, single-threaded design
- values are loaded and saved as whole strings
- mutating commands are automatically persisted to the current file
- `LOAD` and `SAVE` operate on one current persistence file path
- no transactions, snapshots, or concurrency control

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

### Server Mode (default)

```bash
./build/key_value_store
```

With options:

```bash
./build/key_value_store -h 0.0.0.0 -p 6379 -f my-store.db
```

### CLI Mode

```bash
./build/key_value_store --cli
./build/key_value_store --cli -f my-store.db
```

### Options

| Option | Description | Default |
|--------|-------------|---------|
| `-h, --host` | Host to bind | `127.0.0.1` |
| `-p, --port` | Port to listen | `6379` |
| `-f, --file` | Persistence file | `key_value_store.db` |
| `--cli` | Run in CLI mode | off |

### Client

```bash
# Interactive mode
./build/kv_client

# Single command
./build/kv_client SET key value
./build/kv_client GET key
./build/kv_client PING

# Connect to custom server
./build/kv_client -h 192.168.1.100 -p 6379
```

#### Client Options

| Option | Description | Default |
|--------|-------------|---------|
| `-h, --host` | Server host | `127.0.0.1` |
| `-p, --port` | Server port | `6379` |
| `-n, --no-echo` | Disable response formatting | off |

## Example Session

```text
SET language C++
GET language
EXISTS language
COUNT
SAVE
EXIT
```

## Test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Suggested Next Steps

- decide deliberately when concurrency should enter the design
- add transaction support for batch operations
- add TTL (time-to-live) for key expiration
- implement different eviction policies (LRU, LFU)
