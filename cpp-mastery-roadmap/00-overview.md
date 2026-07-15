[中文](00-overview.zh-CN.md) | [English](00-overview.md)

# C++ Mastery Roadmap

This roadmap turns the repository into a staged learning path.
It is not meant to be exhaustive.
It is meant to help you move between concepts, pitfalls, examples, and projects
without getting lost.

## How To Use This Roadmap

- Each stage has four parts:
  core topics, pitfalls, examples, and something to build or improve.
- Do not try to read the whole repository linearly.
- Finish one small cycle at a time:
  learn one idea, inspect one failure mode, then change code.
- Some projects in this roadmap are intentionally incomplete.
  The point is to study and extend them, not just consume them.

## Stage 1: Foundations

Focus:
ownership, lifetime, RAII, `const`, and undefined behavior.

Read:

- [Ownership - Starting from Here](../docs/en/ownership-starts-here.md)
- [Ownership and Lifetime](../docs/en/ownership-and-lifetime.md)
- [RAII and Exception Safety](../docs/en/raii-and-exception-safety.md)
- [The True Semantics of `const`](../docs/en/the-true-semantics-of-const.md)
- [Undefined Behavior](../docs/en/undefined-behavior.md)

Pitfalls:

- [Dangling Pointer](../pitfalls/en/dangling-pointer.md)
- [Returning Reference to Local Object](../pitfalls/en/returning-reference-to-local-object.md)
- [Throwing from Destructor](../pitfalls/en/throwing-from-destructor.md)

Examples:

- [ownership-and-lifetime.cpp](../examples/ownership-and-lifetime.cpp)
- [const-vs-non-const.cpp](../examples/const-vs-non-const.cpp)

Practice:

- Read through the beginner exercise in
  [student-management/v1](01-beginner/student-management/v1)
- Improve one thing:
  input validation, file format design, separation of UI and domain logic, or
  error handling
- See [student-management/v2](01-beginner/student-management/v2) for an
  example of these improvements

## Stage 2: References, Move, and Value Categories

Focus:
reference binding, move semantics, forwarding, copy elision, and interface design.

Read:

- [From Value Categories to Reference Binding](../docs/en/from-value-categories-to-reference-binding.md)
- [Lvalue References](../docs/en/lvalue-references.md)
- [Rvalue References](../docs/en/rvalue-references.md)
- [Lvalue and Rvalue References](../docs/en/lvalue-and-rvalue-references.md)
- [Understanding the Semantics of `std::move`](../docs/en/understanding-the-semantics-of-std::move.md)
- [Understanding Forwarding References and Perfect Forwarding](../docs/en/understanding-forwarding-references-and-perfect-forwarding.md)
- [Copy Elision and Prvalue Materialization](../docs/en/copy-elision-and-prvalue-materialization.md)
- [Noexcept and Move Operations](../docs/en/noexcept-and-move-operations.md)

Pitfalls:

- [Moving from `const` Object](../pitfalls/en/moving-from-const-object.md)
- [Object Slicing](../pitfalls/en/object-slicing.md)
- [Implicit Conversions](../pitfalls/en/implicit-conversions.md)

Examples:

- [move-semantics-example.cpp](../examples/move-semantics-example.cpp)
- [copy_on_write_example.cpp](../examples/copy_on_write_example.cpp)

Practice:

- Revisit the beginner project and make the ownership and interface boundaries
  more explicit
- Identify APIs that should borrow, own, or transfer ownership

## Stage 3: Object Model, Concurrency, and Deeper Semantics

Focus:
virtual dispatch, template instantiation, memory model, ABI boundaries, and
coroutine lifetime.

Read:

- [Virtual Dispatch and Object Model](../docs/en/virtual-dispatch-and-object-model.md)
- [C++ Memory Model and Atomics Basics](../docs/en/cpp-memory-model-and-atomics-basics.md)
- [Template Instantiation and Two-Phase Lookup](../docs/en/template-instantiation-and-two-phase-lookup.md)
- [PImpl and ABI Stability](../docs/en/pimpl-and-abi-stability.md)
- [Coroutines: Lifetime and Suspension Points](../docs/en/coroutines-lifetime-and-suspension-points.md)

Pitfalls:

- [Broken Double-Checked Locking](../pitfalls/en/broken-double-checked-locking.md)
- [The Myth of Thread Safety](../pitfalls/en/the-myth-of-thread-safety.md)
- [Iterator Invalidation](../pitfalls/en/iterator-invalidation.md)
- [Static Initialization Order Fiasco](../pitfalls/en/static-initialization-order-fiasco.md)

Practice:

- Pick one pitfall and write a minimal reproduction case for it
- Then rewrite the code with a safer ownership or synchronization model
- Build one of the Stage 3 projects to practice concurrency and systems programming

### Stage 3 Projects

- [thread-pool](03-advanced/thread-pool/README.md):
  concurrent task execution with futures and synchronization
- [allocator](03-advanced/allocator/README.md):
  fixed-block memory allocator with STL interface
- [json-parser](03-advanced/json-parser/README.md):
  recursive descent parser with type-safe value tree

## Stage 4: Build Larger Things

Focus:
turning semantics into design decisions in real code.

Start with:

- [02 Intermediate Overview](02-intermediate/README.md)

Projects:

- [tiny-http-server](02-intermediate/tiny-http-server/README.md):
  socket handling, request parsing, routing, response construction, fixed-size worker pool
- [key-value-store](02-intermediate/key-value-store/README.md):
  design-stage storage exercise focused on state ownership and persistence boundaries
- [tinyshell](../tinyshell/README.md):
  side project for command parsing and process/system behavior

### Stage 4 Projects (04-stage4)

- [simple-db](04-stage4/simple-db/README.md):
  in-memory database with table management and file persistence
- [chat-server](04-stage4/chat-server/README.md):
  multi-user chat server with rooms and commands
- [web-server](04-stage4/web-server/README.md):
  HTTP server with routing and middleware support

### C++20 Examples

- [cpp20-concepts.cpp](../examples/cpp20-concepts.cpp):
  concepts, requires expressions, constraints
- [cpp20-ranges.cpp](../examples/cpp20-ranges.cpp):
  ranges library, views, adaptors
- [cpp20-span.cpp](../examples/cpp20-span.cpp):
  non-owning views, subspan
- [cpp20-format.cpp](../examples/cpp20-format.cpp):
  std::format string formatting
- [cpp20-coroutines.cpp](../examples/cpp20-coroutines.cpp):
  generators, tasks, async patterns
- [cpp20-consteval.cpp](../examples/cpp20-consteval.cpp):
  consteval, constinit, compile-time evaluation

Suggested progression:

1. Read the stage overview and pick one project with explicit non-goals.
2. Build and inspect the current project state.
3. List the ownership boundaries, lifetime assumptions, and error paths.
4. Add one missing piece:
   tests, better parsing, RAII wrappers, clearer APIs, persistence rules, or
   documentation that matches the implementation.

## Current Status

This roadmap is a living guide, not a finished curriculum.
Some sections are dense, some projects are early-stage, and the repository still
has gaps between design intent and implementation.

That is normal for this project.
The goal is to make those gaps visible and then close them deliberately.

### Stage 5 Projects (05-stage5)

- [task-scheduler](05-stage5/task-scheduler/README.md):
  priority-based task scheduling with delayed and repeating tasks
- [web-framework](05-stage5/web-framework/README.md):
  simple HTTP web framework with routing and middleware

### Stage 6 Projects (06-stage6)

- [connection-pool](06-stage6/connection-pool/README.md):
  generic connection pool for managing reusable resources
- [ring-buffer](06-stage6/ring-buffer/README.md):
  thread-safe circular buffer for producer-consumer patterns

### Stage 7 Projects (07-stage7)

- [thread-safe-cache](07-stage7/thread-safe-cache/README.md):
  generic thread-safe cache with TTL support
- [lru-cache](07-stage7/lru-cache/README.md):
  LRU cache with O(1) operations and eviction

### Stage 8 Projects (08-stage8)

- [object-pool](08-stage8/object-pool/README.md):
  generic object pool for reusing expensive objects
- [publish-subscribe](08-stage8/publish-subscribe/README.md):
  topic-based publish-subscribe messaging system
- [state-machine](08-stage8/state-machine/README.md):
  generic state machine for modeling state transitions

### Stage 9 Projects (09-stage9)

- [command-pattern](09-stage9/command-pattern/README.md):
  command pattern with undo/redo support
- [observer-pattern](09-stage9/observer-pattern/README.md):
  observer pattern for event notification
- [strategy-pattern](09-stage9/strategy-pattern/README.md):
  strategy pattern for algorithm selection

### Stage 10 Projects (10-stage10)

- [decorator-pattern](10-stage10/decorator-pattern/README.md):
  decorator pattern for dynamic behavior composition
- [factory-pattern](10-stage10/factory-pattern/README.md):
  factory pattern for object creation
- [adapter-pattern](10-stage10/adapter-pattern/README.md):
  adapter pattern for interface compatibility

### Stage 11 Projects (11-stage11)

- [builder-pattern](11-stage11/builder-pattern/README.md):
  builder pattern for complex object construction
- [prototype-pattern](11-stage11/prototype-pattern/README.md):
  prototype pattern for object cloning
- [mediator-pattern](11-stage11/mediator-pattern/README.md):
  mediator pattern for object communication

### Stage 12 Projects (12-stage12)

- [flyweight-pattern](12-stage12/flyweight-pattern/README.md):
  flyweight pattern for sharing common data
- [chain-of-responsibility](12-stage12/chain-of-responsibility/README.md):
  chain of responsibility for request handling
- [visitor-pattern](12-stage12/visitor-pattern/README.md):
  visitor pattern for operations on object structures

### Stage 13 Projects (13-stage13)

- [singleton-pattern](13-stage13/singleton-pattern/README.md):
  singleton pattern for single instance management
- [null-object-pattern](13-stage13/null-object-pattern/README.md):
  null object pattern for handling null cases
- [memento-pattern](13-stage13/memento-pattern/README.md):
  memento pattern for state snapshots

### Stage 14 Projects (14-stage14)

- [interpreter-pattern](14-stage14/interpreter-pattern/README.md):
  interpreter pattern for language parsing
- [composite-pattern](14-stage14/composite-pattern/README.md):
  composite pattern for tree structures
- [proxy-pattern](14-stage14/proxy-pattern/README.md):
  proxy pattern for controlled access

### Stage 15 Projects (15-stage15)

- [abstract-factory](15-stage15/abstract-factory/README.md):
  abstract factory for creating families of objects
- [template-method](15-stage15/template-method/README.md):
  template method for algorithm skeleton
- [iterator-pattern](15-stage15/iterator-pattern/README.md):
  iterator pattern for traversing collections

### Stage 16 Projects (16-stage16)

- [double-dispatch](16-stage16/double-dispatch/README.md):
  double dispatch for multi-method polymorphism
- [virtual-constructor](16-stage16/virtual-constructor/README.md):
  virtual constructor for cloning without knowing type
- [object-pool-advanced](16-stage16/object-pool-advanced/README.md):
  advanced object pool with validation

### Stage 17 Projects (17-stage17)

- [command-batch](17-stage17/command-batch/README.md):
  command batch for grouping multiple commands
- [observer-event](17-stage17/observer-event/README.md):
  observer event system for event-driven architecture
- [strategy-cache](17-stage17/strategy-cache/README.md):
  strategy cache with LRU and TTL strategies

### Stage 18 Projects (18-stage18)

- [command-queue](18-stage18/command-queue/README.md):
  thread-safe command queue for async task execution
- [observer-pipeline](18-stage18/observer-pipeline/README.md):
  pipeline for chained data processing
- [strategy-selector](18-stage18/strategy-selector/README.md):
  dynamic strategy selection system

### Stage 19 Projects (19-stage19)

- [command-history](19-stage19/command-history/README.md):
  command history manager for logging and replay
- [observer-filter](19-stage19/observer-filter/README.md):
  observer pattern with event filtering
- [strategy-evaluator](19-stage19/strategy-evaluator/README.md):
  strategy evaluation system for performance comparison

### Stage 20 Projects (20-stage20)

- [command-validator](20-stage20/command-validator/README.md):
  command validation system
- [observer-mediator](20-stage20/observer-mediator/README.md):
  observer pattern combined with mediator
- [strategy-adapter](20-stage20/strategy-adapter/README.md):
  strategy adapter for interface compatibility

### Stage 21 Projects (21-stage21)

- [command-router](21-stage21/command-router/README.md):
  command routing system for dispatching commands
- [observer-bus](21-stage21/observer-bus/README.md):
  event bus for publish-subscribe communication
- [strategy-factory](21-stage21/strategy-factory/README.md):
  factory for creating strategies dynamically

### Stage 22 Projects (22-stage22)

- [command-logger](22-stage22/command-logger/README.md):
  command logging system for tracking operations
- [observer-throttler](22-stage22/observer-throttler/README.md):
  observer with throttled event dispatch
- [strategy-chain](22-stage22/strategy-chain/README.md):
  chain for executing multiple strategies

### Stage 23 Projects (23-stage23)

- [command-queue-v2](23-stage23/command-queue-v2/README.md):
  enhanced command queue with priority support
- [observer-buffer](23-stage23/observer-buffer/README.md):
  observer with buffered event dispatch
- [strategy-decorator](23-stage23/strategy-decorator/README.md):
  decorator pattern for adding behavior to strategies

### Stage 24 Projects (24-stage24)

- [command-snapshot](24-stage24/command-snapshot/README.md):
  command pattern with state snapshots for undo/redo
- [observer-subscription](24-stage24/observer-subscription/README.md):
  observer with RAII subscription management
- [strategy-pipeline](24-stage24/strategy-pipeline/README.md):
  data processing pipeline using strategy pattern

### Stage 25 Projects (25-stage25)

- [command-memento](25-stage25/command-memento/README.md):
  command pattern with memento for state preservation
- [observer-debouncer](25-stage25/observer-debouncer/README.md):
  observer with debounced event dispatch
- [strategy-combinator](25-stage25/strategy-combinator/README.md):
  strategy combinator for combining predicates

### Stage 26 Projects (26-stage26)

- [command-translator](26-stage26/command-translator/README.md):
  translates commands between different formats
- [observer-filter-chain](26-stage26/observer-filter-chain/README.md):
  observer with chained event filters
- [strategy-resolver](26-stage26/strategy-resolver/README.md):
  resolves strategies dynamically based on context

### Stage 27 Projects (27-stage27) — GoF 23 Complete

- [bridge-pattern](27-stage27/bridge-pattern/README.md):
  separates abstraction from implementation
- [facade-pattern](27-stage27/facade-pattern/README.md):
  simplified interface to complex subsystem
- [specification-pattern](27-stage27/specification-pattern/README.md):
  composable business rules
