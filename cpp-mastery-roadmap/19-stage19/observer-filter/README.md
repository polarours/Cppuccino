# Observer Filter

An observer pattern with event filtering.

## Learning Goals

- Understand event filtering
- Practice conditional event handling
- Learn filter composition
- Implement selective notification

## Non-Goals

- Building a production filter system
- Implementing complex filter logic
- Supporting distributed filters

## Features

- Event with type, data, priority
- EventFilter predicate
- FilteredDispatcher for filtering
- Multiple filter support
- Selective notification

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/observer_filter_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "observer_filter.hpp"

observer_filter::FilteredDispatcher dispatcher;
dispatcher.addFilter([](const observer_filter::Event& e) {
    return e.priority > 5;
});

dispatcher.onEvent([](const observer_filter::Event& e) {
    std::cout << e.data << "\n";
});

dispatcher.emit({"click", "low", 3});   // Filtered
dispatcher.emit({"click", "high", 10}); // Passed
```

## Suggested Next Steps

- Add more filter types
- Implement filter chaining
- Add filter statistics
- Implement distributed filtering
