[中文](README_zh-CN.md) | [English](README.md)

# Cppuccino

Cppuccino is a curated bilingual (English/Chinese) C++ learning repository.
It combines concept notes, pitfalls, runnable examples, and project-driven practice
into a single place.

Think of it as a personal C++ study log that is gradually being shaped into a
public learning resource.

> This project reflects the author's current understanding of C++.
> It is still evolving, and corrections or discussion are always welcome.

## Quick Stats

| Category | Count |
|----------|-------|
| Documentation | 42 articles (20 en + 22 zh) |
| Pitfalls | 34 articles (17 en + 17 zh) |
| Examples | 47 runnable examples |
| Projects | 115 projects across 40 stages |
| Tests | 578 (all passing) |

## Start Here

If this is your first visit, use this order:

1. Read the [Roadmap Overview](cpp-mastery-roadmap/00-overview.md).
2. Pick one topic track from the concept docs.
3. Read one related pitfall article.
4. Finish by touching code in an example or project.

### Suggested Entry Points

- **Ownership and lifetime:**
  [Ownership - Starting from Here](docs/en/ownership-starts-here.md),
  [Ownership and Lifetime](docs/en/ownership-and-lifetime.md),
  [RAII and Exception Safety](docs/en/raii-and-exception-safety.md),
  [Dangling Pointer](pitfalls/en/dangling-pointer.md),
  [ownership-and-lifetime.cpp](examples/ownership-and-lifetime.cpp)

- **References, move semantics, and value categories:**
  [Lvalue and Rvalue References](docs/en/lvalue-and-rvalue-references.md),
  [Understanding the Semantics of `std::move`](docs/en/understanding-the-semantics-of-std::move.md),
  [Forwarding References and Perfect Forwarding](docs/en/understanding-forwarding-references-and-perfect-forwarding.md),
  [move-semantics-example.cpp](examples/move-semantics-example.cpp)

- **Object model and deeper semantics:**
  [Virtual Dispatch and Object Model](docs/en/virtual-dispatch-and-object-model.md),
  [Undefined Behavior](docs/en/undefined-behavior.md),
  [C++ Memory Model and Atomics Basics](docs/en/cpp-memory-model-and-atomics-basics.md),
  [virtual-dispatch-and-slicing.cpp](examples/virtual-dispatch-and-slicing.cpp)

## Roadmap Overview

The [Mastery Roadmap](cpp-mastery-roadmap/00-overview.md) is a staged learning path:

| Stages | Theme | Projects |
|--------|-------|----------|
| 1–7 | C++ Foundations to Advanced | 7 |
| 8–27 | Design Patterns (GoF 23 + Specification) | 60 |
| 28–31 | Architecture Patterns | 12 |
| 32–33 | Concurrency Patterns | 6 |
| 34 | C++ Specific Patterns (CRTP, SFINAE, Variadic) | 3 |
| 35 | Memory Patterns | 3 |
| 36 | Modern C++ (Coroutines, Ranges, Concepts) | 3 |
| 37 | Error Handling Patterns | 3 |
| 38 | Testing Patterns | 3 |
| 39 | Examples & Pitfalls Enhancement | — |
| 40 | C++23 Features | 3 |

## Repository Map

### Documentation

- [docs/en](docs/en) and [docs/中文版](docs/中文版):
  Concept-focused essays, usually one idea per document
- [pitfalls/en](pitfalls/en) and [pitfalls/中文版](pitfalls/中文版):
  Common mistakes, invalid assumptions, and review-oriented notes

### Examples

- [examples](examples):
  47 runnable code samples covering:
  - Core concepts: ownership, move semantics, smart pointers, RAII
  - Modern C++: lambda, constexpr, structured bindings, ranges
  - C++20: concepts, ranges, span, format, coroutines
  - C++23: fold expressions, CRTP, SFINAE, type traits
  - Pitfall reproductions: dangling references, object slicing, iterator invalidation

### Projects

#### Stage 1: Beginner

- [student-management/v1](cpp-mastery-roadmap/01-beginner/student-management/v1):
  Basic student management system
- [student-management/v2](cpp-mastery-roadmap/01-beginner/student-management/v2):
  Improved version with better I/O and tests

#### Stage 2: Intermediate

- [key-value-store](cpp-mastery-roadmap/02-intermediate/key-value-store):
  Complete key-value store with protocol, server, client, and concurrency
- [tiny-http-server](cpp-mastery-roadmap/02-intermediate/tiny-http-server):
  HTTP server with routing and thread pool

#### Stage 3: Advanced

- [thread-pool](cpp-mastery-roadmap/03-advanced/thread-pool):
  Thread pool with futures and synchronization
- [allocator](cpp-mastery-roadmap/03-advanced/allocator):
  Fixed-block memory allocator with STL interface
- [json-parser](cpp-mastery-roadmap/03-advanced/json-parser):
  Recursive descent JSON parser
- [http-parser](cpp-mastery-roadmap/03-advanced/http-parser):
  HTTP request/response parser
- [smart-pointer](cpp-mastery-roadmap/03-advanced/smart-pointer):
  Custom smart pointer implementation
- [event-loop](cpp-mastery-roadmap/03-advanced/event-loop):
  Event loop with priority queue for timers

#### Stages 4–7: More Practice

- [simple-db](cpp-mastery-roadmap/04-stage4/simple-db):
  File-based key-value database
- [chat-server](cpp-mastery-roadmap/04-stage4/chat-server):
  Multi-client chat server
- [web-server](cpp-mastery-roadmap/04-stage4/web-server):
  HTTP web server with static files

#### Stages 8–27: Design Patterns

- [GoF 23 patterns](cpp-mastery-roadmap/00-overview.md#stage-8-projects-8-stage8):
  All 23 Gang-of-Four patterns + Specification
- **Command pattern variants**: command-history, command-logger, command-memento, command-router, command-snapshot, command-translator, command-validator
- **Observer pattern variants**: observer-bus, observer-buffer, observer-debouncer, observer-filter-chain, observer-mediator, observer-subscription, observer-throttler
- **Strategy pattern variants**: strategy-adapter, strategy-chain, strategy-combinator, strategy-decorator, strategy-evaluator, strategy-factory, strategy-pipeline, strategy-resolver

#### Stages 28–31: Architecture Patterns

- [mvc-pattern](cpp-mastery-roadmap/28-stage28/mvc-pattern):
  Model-View-Controller architecture
- [repository-pattern](cpp-mastery-roadmap/28-stage28/repository-pattern):
  Data access abstraction
- [cqrs-pattern](cpp-mastery-roadmap/28-stage28/cqrs-pattern):
  Command Query Responsibility Segregation
- [event-sourcing](cpp-mastery-roadmap/29-stage29/event-sourcing):
  Event-based state management and replay
- [circuit-breaker](cpp-mastery-roadmap/29-stage29/circuit-breaker):
  Fault tolerance pattern
- [middleware-pipeline](cpp-mastery-roadmap/29-stage29/middleware-pipeline):
  Chained request/response processing
- [service-locator](cpp-mastery-roadmap/30-stage30/service-locator):
  Centralized service registry
- [unit-of-work](cpp-mastery-roadmap/30-stage30/unit-of-work):
  Transaction-like change tracking
- [active-record](cpp-mastery-roadmap/30-stage30/active-record):
  ORM pattern
- [type-object](cpp-mastery-roadmap/31-stage31/type-object):
  Runtime type system
- [delegation](cpp-mastery-roadmap/31-stage31/delegation):
  Behavior delegation
- [balking](cpp-mastery-roadmap/31-stage31/balking):
  Guard-check pattern

#### Stages 32–33: Concurrency Patterns

- [guarded-suspension](cpp-mastery-roadmap/32-stage32/guarded-suspension):
  Wait for condition before proceeding
- [producer-consumer](cpp-mastery-roadmap/32-stage32/producer-consumer):
  Bounded buffer with thread-safe push/pop
- [double-buffer](cpp-mastery-roadmap/32-stage32/double-buffer):
  Smooth data transition
- [read-write-lock](cpp-mastery-roadmap/33-stage33/read-write-lock):
  Shared/exclusive locking
- [lock-free-queue](cpp-mastery-roadmap/33-stage33/lock-free-queue):
  Wait-free queue
- [thread-pool-advanced](cpp-mastery-roadmap/33-stage33/thread-pool-advanced):
  Task-based thread pool with futures

#### Stages 34–38: C++, Memory, Modern C++, Error, Testing

- [crtp-pattern](cpp-mastery-roadmap/34-stage34/crtp-pattern):
  Curiously Recurring Template Pattern
- [sfinae-pattern](cpp-mastery-roadmap/34-stage34/sfinae-pattern):
  Compile-time dispatch
- [varargs-pattern](cpp-mastery-roadmap/34-stage34/varargs-pattern):
  Variadic templates
- [memory-pool](cpp-mastery-roadmap/35-stage35/memory-pool):
  Pre-allocated block pool
- [arena-allocator](cpp-mastery-roadmap/35-stage35/arena-allocator):
  Bump allocator
- [slab-allocator](cpp-mastery-roadmap/35-stage35/slab-allocator):
  Size-classified cache
- [coroutine-pattern](cpp-mastery-roadmap/36-stage36/coroutine-pattern):
  C++20 coroutines
- [ranges-pattern](cpp-mastery-roadmap/36-stage36/ranges-pattern):
  Range pipelines
- [concepts-pattern](cpp-mastery-roadmap/36-stage36/concepts-pattern):
  C++20 Concepts
- [result-type](cpp-mastery-roadmap/37-stage37/result-type):
  Type-safe error handling
- [error-code](cpp-mastery-roadmap/37-stage37/error-code):
  Enum-based error reporting
- [expected-pattern](cpp-mastery-roadmap/37-stage37/expected-pattern):
  Value-or-error with monadic ops
- [mock-objects](cpp-mastery-roadmap/38-stage38/mock-objects):
  Test doubles
- [test-fixture](cpp-mastery-roadmap/38-stage38/test-fixture):
  Reusable test setup
- [parameterized-tests](cpp-mastery-roadmap/38-stage38/parameterized-tests):
  Data-driven tests

#### Stage 40: C++23 Features

- [std-print](cpp-mastery-roadmap/40-stage40/std-print):
  Type-safe formatted output
- [deducing-this](cpp-mastery-roadmap/40-stage40/deducing-this):
  C++23 explicit object parameter
- [zip-range](cpp-mastery-roadmap/40-stage40/zip-range):
  Parallel iteration

### Other

- [tinyshell](tinyshell):
  Unix shell with job control, signal handling, and variable expansion

## How To Use This Repo

- **Read by topic** if you want a concise explanation of one C++ idea.
- **Read by roadmap** if you want a more structured learning path.
- **Use the pitfalls folder** as a review checklist when writing or reviewing code.
- **Jump between docs and examples** when you want the semantic explanation and the code sample together.
- **Build the projects** to practice applying concepts in real code.

## Building Projects

Most projects use CMake. To build and test any project:

```bash
cd cpp-mastery-roadmap/<stage>/<project>
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Some projects require C++20 or C++23 — check the CMakeLists.txt for `CMAKE_CXX_STANDARD`.

## Project Philosophy

- One document should focus on one central idea.
- Content should stay concise, technically grounded, and reviewable.
- Explanation matters more than prescription.
- Ownership, lifetime, semantics, and tradeoffs matter more than folklore.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

Technical corrections, simplifications, better examples, and thoughtful
counterexamples are especially useful contributions.
