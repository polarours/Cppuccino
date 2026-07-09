# State Machine

A generic state machine implementation for modeling state transitions.

## Learning Goals

- Understand state machine design patterns
- Practice template-based generic programming
- Learn state transition management
- Implement event-driven state changes

## Non-Goals

- Building a production state machine framework
- Implementing hierarchical state machines
- Supporting complex state machine diagrams

## Features

- Generic template-based state machine
- State transitions with actions
- Query available events
- Set state directly
- Thread-safe by design

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/state_machine_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "state_machine.hpp"

enum class State { Idle, Running, Stopped };
enum class Event { Start, Stop };

state_machine::StateMachine<State, Event> sm;
sm.setState(State::Idle);
sm.addTransition(State::Idle, Event::Start, State::Running);
sm.addTransition(State::Running, Event::Stop, State::Stopped);

sm.handleEvent(Event::Start);  // State: Running
sm.handleEvent(Event::Stop);   // State: Stopped
```

## Suggested Next Steps

- Add hierarchical state machines
- Implement state entry/exit actions
- Add guard conditions for transitions
- Implement state machine visualization
