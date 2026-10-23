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

## Shutting Down

`stop()` has to reach a thread that is blocked in `accept()` or in
`recv()`. On Linux neither of them wakes up when a flag is set, so the
sockets themselves are shut down.

The ownership is what keeps that safe:

- `start()` creates the listening socket and closes it again when the
  accept loop ends. `stop()` only shuts it down, and does so under the
  lock. A descriptor that is closed while another thread still holds its
  number may already belong to somebody else.
- A client that is accepted at the very moment the server is stopped does
  not get a thread. The running flag and the thread list are read under
  the same lock, so such a connection is closed instead of ending up in a
  list that nobody joins.
- The client threads are taken out of the list under the lock and joined
  outside of it, because a client thread needs the lock itself to
  broadcast its farewell message.

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
