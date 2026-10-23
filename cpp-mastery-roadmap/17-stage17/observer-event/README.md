# Observer Event System

An observer event system for event-driven architecture.

## Learning Goals

- Understand event-driven architecture
- Practice event dispatching
- Learn handler registration
- Implement event emission

## Non-Goals

- Building a production event system
- Implementing complex event filtering
- Supporting distributed events

## Features

- Event struct with type, source, data
- EventHandler callback function
- EventDispatcher for registration and emission
- Multiple handlers per event type
- Handler count tracking

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/observer_event_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "observer_event.hpp"

observer_event::EventDispatcher dispatcher;
dispatcher.on("click", [](const observer_event::Event& e) {
    std::cout << "Clicked: " << e.data << "\n";
});

dispatcher.emit({"click", "button", "clicked"});
```

## Suggested Next Steps

- Add more event types
- Implement event filtering
- Add event history
- Implement distributed events
