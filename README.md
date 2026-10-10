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
| Documentation | 134 articles (67 en + 67 zh) |
| Pitfalls | 74 articles (37 en + 37 zh) |
| Examples | 120 runnable examples |
| Projects | 150 projects across 43 stages |
| Tests | 848 (all passing) |

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
  [Understanding the C++ Object Model](docs/en/understanding-the-cpp-object-model.md),
  [Undefined Behavior](docs/en/undefined-behavior.md),
  [C++ Memory Model and Atomics Basics](docs/en/cpp-memory-model-and-atomics-basics.md),
  [virtual-dispatch-and-slicing.cpp](examples/virtual-dispatch-and-slicing.cpp)

## Roadmap Overview

The [Mastery Roadmap](cpp-mastery-roadmap/00-overview.md) is a staged learning path:

| Stages | Theme | Projects |
|--------|-------|----------|
| 1–7 | C++ Foundations to Advanced | 35 |
| 8–27 | Design Patterns (GoF 23 + Specification) | 61 |
| 28–31 | Architecture Patterns | 12 |
| 32–33 | Concurrency Patterns | 7 |
| 34 | C++ Specific Patterns (CRTP, SFINAE, Variadic) | 4 |
| 35 | Memory Patterns | 4 |
| 36 | Modern C++ (Coroutines, Ranges, Concepts) | 3 |
| 37 | Error Handling Patterns | 4 |
| 38 | Testing Patterns | 4 |
| 39 | Content Enhancement (Parser / Diff Tools) | 3 |
| 40 | C++23 Features | 4 |
| 41 | Capstone (HTTP Framework, Router, Template Engine) | 3 |
| 42 | Performance Patterns | 3 |
| 43 | Toolchain Patterns (CMake, CI/CD, Package Manager) | 3 |

## Repository Map

### Documentation

- [docs/en](docs/en) and [docs/中文版](docs/中文版):
  Concept-focused essays, usually one idea per document
- [pitfalls/en](pitfalls/en) and [pitfalls/中文版](pitfalls/中文版):
  Common mistakes, invalid assumptions, and review-oriented notes

### Examples

- [examples](examples):
  120 runnable code samples covering:
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
- [student-management/v3](cpp-mastery-roadmap/01-beginner/student-management/v3):
  Borrowing parameters with std::string_view

#### Stage 2: Intermediate

- [key-value-store](cpp-mastery-roadmap/02-intermediate/key-value-store):
  Complete key-value store with protocol, server, client, and concurrency
- [tiny-http-server](cpp-mastery-roadmap/02-intermediate/tiny-http-server):
  HTTP server with routing and thread pool
- [copy-move-demo](cpp-mastery-roadmap/02-intermediate/copy-move-demo):
  Copy/move semantics with observable constructors
- [reference-binding-demo](cpp-mastery-roadmap/02-intermediate/reference-binding-demo):
  Reference binding and lifetime extension rules
- [trie](cpp-mastery-roadmap/02-intermediate/trie):
  Prefix tree with word/prefix counts
- [union-find](cpp-mastery-roadmap/02-intermediate/union-find):
  Disjoint set union with path compression
- [binary-heap](cpp-mastery-roadmap/02-intermediate/binary-heap):
  Array-backed heap with bottom-up heapify

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
- [static-init-demo](cpp-mastery-roadmap/03-advanced/static-init-demo):
  Static initialization order fiasco and the fix
- [skip-list](cpp-mastery-roadmap/03-advanced/skip-list):
  Probabilistic ordered set with randomized levels
- [csv-parser](cpp-mastery-roadmap/03-advanced/csv-parser):
  RFC-4180 quoting rules with roundtrip formatting
- [base64-codec](cpp-mastery-roadmap/03-advanced/base64-codec):
  RFC 4648 encode/decode with strict padding validation

#### Stages 4–7: More Practice

- [simple-db](cpp-mastery-roadmap/04-stage4/simple-db):
  File-based key-value database
- [chat-server](cpp-mastery-roadmap/04-stage4/chat-server):
  Multi-client chat server
- [web-server](cpp-mastery-roadmap/04-stage4/web-server):
  HTTP web server with static files
- [mini-redis](cpp-mastery-roadmap/04-stage4/mini-redis):
  In-memory KV store with RESP-style replies and lazy TTL
- [task-scheduler](cpp-mastery-roadmap/05-stage5/task-scheduler):
  Priority/delayed/repeating task scheduler
- [timer-wheel](cpp-mastery-roadmap/05-stage5/timer-wheel):
  O(1) tick-based timer scheduling
- [blocking-queue](cpp-mastery-roadmap/06-stage6/blocking-queue):
  Bounded MPMC queue with dual condition variables
- [lfu-cache](cpp-mastery-roadmap/07-stage7/lfu-cache):
  Least-frequently-used eviction with LRU tie-breaking
- [bloom-filter](cpp-mastery-roadmap/07-stage7/bloom-filter):
  Probabilistic membership with double hashing

#### Stages 8–27: Design Patterns

- [GoF 23 patterns](cpp-mastery-roadmap/00-overview.md#stage-8-projects-8-stage8):
  All 23 Gang-of-Four patterns + Specification
- **Command pattern variants**: command-history, command-logger, command-memento, command-router, command-snapshot, command-translator, command-validator
- **Observer pattern variants**: observer-bus, observer-buffer, observer-debouncer, observer-filter-chain, observer-mediator, observer-subscription, observer-throttler
- **Strategy pattern variants**: strategy-adapter, strategy-chain, strategy-combinator, strategy-decorator, strategy-evaluator, strategy-factory, strategy-pipeline, strategy-resolver
- [rate-limiter](cpp-mastery-roadmap/08-stage8/rate-limiter):
  Token bucket, fixed window, and sliding window algorithms

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
- [actor-mailbox](cpp-mastery-roadmap/33-stage33/actor-mailbox):
  Bounded mailbox with a single-consumer actor loop

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
- [type-erasure](cpp-mastery-roadmap/34-stage34/type-erasure):
  Hand-rolled std::function-style wrapper (Concept/Holder)
- [allocation-tracker](cpp-mastery-roadmap/35-stage35/allocation-tracker):
  Process-wide allocation counters and counting allocator
- [error-category](cpp-mastery-roadmap/37-stage37/error-category):
  Custom std::error_code category mapped to std::errc
- [golden-file-testing](cpp-mastery-roadmap/38-stage38/golden-file-testing):
  Snapshot assertions with line diffs and UPDATE_GOLDEN flow

#### Stage 39: Content Enhancement

- [markdown-parser](cpp-mastery-roadmap/39-stage39/markdown-parser):
  Block-level markdown parsing with inline link extraction
- [link-checker](cpp-mastery-roadmap/39-stage39/link-checker):
  Relative link validation with lexical path normalization
- [text-diff](cpp-mastery-roadmap/39-stage39/text-diff):
  LCS line diff with unified-style rendering

#### Stage 40: C++23 Features

- [std-print](cpp-mastery-roadmap/40-stage40/std-print):
  Type-safe formatted output
- [deducing-this](cpp-mastery-roadmap/40-stage40/deducing-this):
  C++23 explicit object parameter
- [zip-range](cpp-mastery-roadmap/40-stage40/zip-range):
  Parallel iteration
- [generator-pipeline](cpp-mastery-roadmap/40-stage40/generator-pipeline):
  Lazy std::generator pipelines with early-exit propagation

#### Stage 41: Capstone Project

- [mini-http-framework](cpp-mastery-roadmap/41-stage41/mini-http-framework):
  Complete HTTP framework combining 10 design patterns
- [http-router](cpp-mastery-roadmap/41-stage41/http-router):
  Method+path routing with :param extraction
- [template-engine](cpp-mastery-roadmap/41-stage41/template-engine):
  Mustache-like sections with context chaining

#### Stage 42: Performance Patterns

- [cache-friendly](cpp-mastery-roadmap/42-stage42/cache-friendly):
  AoS vs SoA data layout for cache performance
- [simd-pattern](cpp-mastery-roadmap/42-stage42/simd-pattern):
  Manual SIMD-style vectorization
- [memory-alignment](cpp-mastery-roadmap/42-stage42/memory-alignment):
  Cache-line alignment and aligned allocation

#### Stage 43: Toolchain Patterns

- [cmake-pattern](cpp-mastery-roadmap/43-stage43/cmake-pattern):
  Programmatic CMakeLists.txt generation
- [package-manager](cpp-mastery-roadmap/43-stage43/package-manager):
  Dependency resolution and package management
- [ci-cd-pattern](cpp-mastery-roadmap/43-stage43/ci-cd-pattern):
  Pipeline pattern for build/test/deploy workflows

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
