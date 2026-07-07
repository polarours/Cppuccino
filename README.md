[中文](README_zh-CN.md) | [English](README.md)

# Cppuccino

Cppuccino is a curated C++ learning repository.
It combines concept notes, pitfalls, runnable examples, and project-driven practice
into a single place.

Think of it as a personal C++ study log that is gradually being shaped into a
public learning resource.

> This project reflects the author's current understanding of C++.
> It is still evolving, and corrections or discussion are always welcome.

## Quick Stats

| Category | Count |
|----------|-------|
| Documentation | 41 articles (20 en + 21 zh) |
| Pitfalls | 30 articles (15 en + 15 zh) |
| Examples | 34 runnable examples |
| Projects | 4 stages (beginner → advanced) |
| Tests | 165+ (all passing) |
| Glossary | 100+ terms (en + zh) |

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

## Repository Map

### Documentation

- [docs/en](docs/en) and [docs/中文版](docs/中文版):
  Concept-focused essays, usually one idea per document
- [pitfalls/en](pitfalls/en) and [pitfalls/中文版](pitfalls/中文版):
  Common mistakes, invalid assumptions, and review-oriented notes
- [Glossary](docs/glossary.md) / [术语表](docs/中文版/术语表.md):
  Quick reference for C++ terms

### Examples

- [examples](examples):
  34 runnable code samples covering:
  - Core concepts: ownership, move semantics, smart pointers
  - Modern C++: lambda, constexpr, structured bindings, ranges
  - C++20: concepts, ranges, span, format
  - Pitfall reproductions: dangling references, object slicing

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
- [tinyshell](tinyshell):
  Unix shell with job control and signal handling

#### Stage 3: Advanced

- [thread-pool](cpp-mastery-roadmap/03-advanced/thread-pool):
  Thread pool with futures and synchronization
- [allocator](cpp-mastery-roadmap/03-advanced/allocator):
  Fixed-block memory allocator with STL interface
- [json-parser](cpp-mastery-roadmap/03-advanced/json-parser):
  Recursive descent JSON parser
- [http-parser](cpp-mastery-roadmap/03-advanced/http-parser):
  HTTP request/response parser

## How To Use This Repo

- **Read by topic** if you want a concise explanation of one C++ idea.
- **Read by roadmap** if you want a more structured learning path.
- **Use the pitfalls folder** as a review checklist when writing or reviewing code.
- **Jump between docs and examples** when you want the semantic explanation and the code sample together.
- **Build the projects** to practice applying concepts in real code.

## Project Philosophy

- One document should focus on one central idea.
- Content should stay concise, technically grounded, and reviewable.
- Explanation matters more than prescription.
- Ownership, lifetime, semantics, and tradeoffs matter more than folklore.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

Technical corrections, simplifications, better examples, and thoughtful
counterexamples are especially useful contributions.
