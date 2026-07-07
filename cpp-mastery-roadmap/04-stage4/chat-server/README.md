# Chat Server

A simple multi-user chat server with rooms and commands.

## Learning Goals

- Understand network programming and sockets
- Practice concurrent client handling with threads
- Learn message broadcasting and routing
- Implement user management and rooms

## Non-Goals

- Building a production chat server
- Implementing encryption or authentication
- Supporting WebSocket or HTTP

## Features

- Multi-user chat with TCP sockets
- Room-based messaging
- User join/leave notifications
- Commands: /join, /leave, /rooms, /users, /quit
- Thread-safe operation

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/chat_server_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```bash
# Terminal 1
./build/chat_server_example

# Terminal 2
telnet localhost 8080
> Alice
> Hello everyone!
> /join general
> /users
```

## Suggested Next Steps

- Add private messaging
- Implement message history
- Add file sharing support
- Implement user authentication
