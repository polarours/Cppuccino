# Observer Pattern

An observer pattern implementation for event notification.

## Learning Goals

- Understand observer pattern design
- Practice weak_ptr for preventing cycles
- Learn event notification systems
- Implement subscriber management

## Non-Goals

- Building a production event system
- Implementing distributed observers
- Supporting complex event filtering

## Features

- Template-based subject and observer
- Multiple observers per subject
- Attach/detach operations
- Weak pointer to prevent cycles
- Automatic cleanup of expired observers

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/observer_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "observer_pattern.hpp"

observer_pattern::Subject<int> subject;

auto observer = std::make_shared<observer_pattern::Logger<int>>();
subject.attach(observer);

subject.setValue(42);  // Logger receives notification
```

## Suggested Next Steps

- Add event filtering
- Implement priority-based notification
- Add asynchronous notification
- Implement distributed observers
