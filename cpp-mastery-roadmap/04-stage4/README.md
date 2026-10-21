# Stage 4: Building Larger Systems

The goal of this stage is to turn what you know about semantics into design
decisions in real code.

## Learning Goals

- Understand the design trade-offs of a larger project
- Practise modularisation and interface design
- Learn error handling and resource management
- Apply C++ features in realistic settings

## Project Overview

### simple-db

An in-memory database with table management and file persistence.

**Focus:**
- data modelling and storage design
- working with variant types (`std::variant`)
- file I/O and serialisation
- implementing CRUD operations

**Features:**
- create and drop tables
- insert, query, update and delete rows
- several data types (int, double, string, bool, null)
- save to and load from a file

**Build and run:**
```bash
cd simple-db
cmake -S . -B build
cmake --build build
./build/simple_db_example
```

### chat-server

A multi-user chat server with rooms and commands.

**Focus:**
- network programming and sockets
- concurrent handling with threads
- message broadcasting and routing
- user management and the room system

**Features:**
- multi-user TCP chat
- room management
- join and leave notifications
- a command set: /join, /leave, /rooms, /users, /quit

**Build and run:**
```bash
cd chat-server
cmake -S . -B build
cmake --build build
./build/chat_server_example
```

### web-server

An HTTP server with routing and middleware.

**Focus:**
- the HTTP protocol and its requests and responses
- socket programming and concurrency
- the routing and middleware patterns
- serving static files

**Features:**
- HTTP request parsing
- route registration (GET, POST, PUT, DELETE)
- middleware support
- static file serving
- thread-based concurrency

**Build and run:**
```bash
cd web-server
cmake -S . -B build
cmake --build build
./build/web_server_example
```

## Suggested Path

1. **Pick one project** and start there
2. **Read the code** and understand how it works today
3. **Identify the design decisions**: ownership boundaries, lifetime
   assumptions, error paths
4. **Add one thing**: better error handling, more tests, documentation
5. **Refactor** so the code reads more clearly and is easier to maintain

## Non-Goals

- Building production systems
- Implementing a complete feature set
- Chasing maximum performance

These projects are learning tools, not finished products.
