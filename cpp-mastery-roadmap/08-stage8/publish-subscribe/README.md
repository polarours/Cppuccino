# Publish-Subscribe

A simple publish-subscribe messaging system.

## Learning Goals

- Understand publish-subscribe pattern
- Practice callback-based event handling
- Learn topic-based message routing
- Implement subscriber management

## Non-Goals

- Building a production message broker
- Implementing message persistence
- Supporting distributed messaging

## Features

- Topic-based publish/subscribe
- Multiple subscribers per topic
- Subscribe/unsubscribe operations
- Thread-safe operations

## Thread Safety

The subscribers that are there when a message is published are the ones
that receive it. They are called with the lock released, so a subscriber
may unsubscribe itself, subscribe to another topic or publish again
without waiting for itself.
- Message ordering

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/pubsub_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "pubsub.hpp"

pubsub::Publisher<std::string> pub;

auto id = pub.subscribe("news", [](const std::string& msg) {
    std::cout << "News: " << msg << "\n";
});

pub.publish("news", "Breaking news!");

pub.unsubscribe(id);
```

## Suggested Next Steps

- Add message filtering
- Implement message queuing
- Add subscriber priorities
- Implement distributed pub-sub
