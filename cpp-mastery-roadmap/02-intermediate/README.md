# 02 Intermediate

This stage is for learners who are already comfortable with basic C++ syntax,
classes, functions, references, and standard containers, and now need practice
turning language knowledge into design decisions.

The goal here is not to learn "more features" in isolation.
The goal is to make ownership, lifetime, error handling, interfaces, and
concurrency visible inside small but meaningful projects.

## What This Stage Trains

- turning core language semantics into API and module boundaries
- making ownership and lifetime assumptions explicit
- reasoning about resource management beyond plain memory
- writing code that is small enough to understand and large enough to refactor
- reading incomplete systems and extending them deliberately

## What This Stage Is Not

- not a production-systems course
- not a full networking, storage, or OS curriculum
- not a place to chase maximum performance before the design is clear
- not a collection of polished final products

This stage is intentionally project-driven.
Some projects are partial on purpose.
You are expected to inspect the current design, identify missing pieces, and
extend them in small, reviewable steps.

## Projects

### [tiny-http-server](tiny-http-server/README.md)

Status:
implemented as a minimal intermediate exercise

What it is for:

- socket lifecycle and resource handling
- request parsing boundaries
- routing and response construction
- fixed-size thread pool basics
- separating transport, parsing, and dispatch responsibilities

What it is not trying to do:

- implement a full HTTP server
- cover persistent connections, streaming, or production hardening
- optimize for throughput before semantics are stable

### [key-value-store](key-value-store/README.md)

Status:
implemented as a minimal intermediate exercise

What it is for:

- data ownership inside a longer-lived service
- persistence format design
- API contracts for read/write operations
- command parsing and state management
- testing stateful behavior

What it is not trying to do:

- compete with real databases
- hide storage semantics behind a vague interface
- jump straight to distributed systems concerns

## Recommended Working Style

For each project in this stage:

1. Write down the ownership boundaries first.
2. State the non-goals before adding features.
3. Add one capability at a time.
4. Keep the implementation small enough that tests and documentation can stay in sync.

## Current Direction

Both `tiny-http-server` and `key-value-store` are concrete projects to study
and extend.

Each project offers different practice:

- `tiny-http-server` focuses on socket lifecycle, request parsing, and concurrency.
- `key-value-store` focuses on state ownership, persistence, and command handling.
