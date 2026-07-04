[中文](README_zh-CN.md) | [English](README.md)

# Cppuccino

Cppuccino is a curated C++ learning repository.
It combines concept notes, pitfalls, runnable examples, and project-driven practice
into a single place.

Think of it as a personal C++ study log that is gradually being shaped into a
public learning resource.

> This project reflects the author's current understanding of C++.
> It is still evolving, and corrections or discussion are always welcome.

## Start Here

If this is your first visit, use this order:

1. Read the [Roadmap Overview](cpp-mastery-roadmap/00-overview.md).
2. Pick one topic track from the concept docs.
3. Read one related pitfall article.
4. Finish by touching code in an example or project.

### Suggested Entry Points

- Ownership and lifetime:
  [Ownership - Starting from Here](docs/en/ownership-starts-here.md),
  [Ownership and Lifetime](docs/en/ownership-and-lifetime.md),
  [RAII and Exception Safety](docs/en/raii-and-exception-safety.md),
  [Dangling Pointer](pitfalls/en/dangling-pointer.md),
  [Dangling `std::string_view`](pitfalls/en/dangling-string-view.md),
  [ownership-and-lifetime.cpp](examples/ownership-and-lifetime.cpp)
- References, move semantics, and value categories:
  [Lvalue and Rvalue References](docs/en/lvalue-and-rvalue-references.md),
  [Understanding the Semantics of `std::move`](docs/en/understanding-the-semantics-of-std::move.md),
  [Forwarding References and Perfect Forwarding](docs/en/understanding-forwarding-references-and-perfect-forwarding.md),
  [Moving from `const` Object](pitfalls/en/moving-from-const-object.md),
  [move-semantics-example.cpp](examples/move-semantics-example.cpp)
- Object model and deeper semantics:
  [Virtual Dispatch and Object Model](docs/en/virtual-dispatch-and-object-model.md),
  [Undefined Behavior](docs/en/undefined-behavior.md),
  [C++ Memory Model and Atomics Basics](docs/en/cpp-memory-model-and-atomics-basics.md),
  [Static Initialization Order Fiasco](pitfalls/en/static-initialization-order-fiasco.md),
  [Broken Double-Checked Locking](pitfalls/en/broken-double-checked-locking.md)

## Repository Map

- [docs/en](docs/en) and [docs/中文版](docs/中文版):
  concept-focused essays, usually one idea per document
- [pitfalls/en](pitfalls/en) and [pitfalls/中文版](pitfalls/中文版):
  common mistakes, invalid assumptions, and review-oriented notes
- [examples](examples):
  small code samples that support selected articles
- [cpp-mastery-roadmap](cpp-mastery-roadmap):
  staged project practice from fundamentals to larger systems exercises
- [tinyshell](tinyshell):
  a side project / sandbox for additional systems programming practice

## How To Use This Repo

- Read by topic if you want a concise explanation of one C++ idea.
- Read by roadmap if you want a more structured learning path.
- Use the pitfalls folder as a review checklist when writing or reviewing code.
- Jump between docs and examples when you want the semantic explanation and the
  code sample together.

## Project Philosophy

- One document should focus on one central idea.
- Content should stay concise, technically grounded, and reviewable.
- Explanation matters more than prescription.
- Ownership, lifetime, semantics, and tradeoffs matter more than folklore.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

Technical corrections, simplifications, better examples, and thoughtful
counterexamples are especially useful contributions.
