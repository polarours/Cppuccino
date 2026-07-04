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
