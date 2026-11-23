# Changelog

Notable changes to Cppuccino. This file is the short, readable index for
readers and for stable references.

## [Unreleased]

### Changed

- A canonical bilingual **terminology glossary** at `docs/i18n/terminology.md`,
  enforced by `tools/check_terminology.py` and published in the site nav. AI or
  human translations now share one vocabulary and update incrementally.
- Read mode now has a three-theme switcher in the header: **CAPPUCCINO**
  (the original warm palette), **ARCADE** (CRT dark with pixel headings),
  and **RETRO CLASSIC** (paper and copper). The choice is saved locally.

### Added

- A **Lifetime Timeline lab** at `/lab/`: a step-through, bilingual widget that
  shows an object dying and a reference becoming dangling, with a real ASan
  report in the failing scenario. It is a separate static page, so the
  documentation stays readable without JavaScript.
- A **Dangling Pointer boss fight** at `/arcade/boss/`: a split-screen battle
  where the player writes a small C++ lifetime spell to control the hero.
  Wrong lifetimes trigger a glitch and a real `-fsanitize=address` report;
  clearing the boss feeds XP into the existing arcade progress.
- A **Play mode / arcade** at `/arcade/`: a level-select shell over the real
  articles, pitfalls, examples and tinyshell labs. Quests are generated from
  the repository by `tools/build_site.py`; progress lives only in the browser.
  Two switchable themes (ARCADE / RETRO CLASSIC) share one layout.
- A **documentation site** built from the existing Markdown by
  `tools/build_site.py` and MkDocs Material, deployed to GitHub Pages by the
  `Pages` workflow. No article is duplicated; the landing pages and nav are
  generated from the repository, and their numbers come from `check_stats.py`.
- A **devcontainer / GitHub Codespaces** environment: GCC 15, clang, CMake,
  Python and the C++ VS Code extensions, with a `postCreateCommand` that
  initialises the tinyshell submodule and checks the README statistics.
  "Start Here" and `CONTRIBUTING.md` link to it.
- A **Clang portability** CI job: the examples and document examples are
  compiled and run with `clang++` in addition to GCC. Code that only one
  compiler accepts is not good teaching material.
- This changelog.

### Fixed

- `examples/constexpr.cpp` called `std::sqrt` from a `constexpr` function,
  which GCC accepts as an extension and clang rejects. The squared distance
  is computed at compile time; the square root happens at run time.
- `check_examples.py` now recognises clang's "missing header" and "unsupported
  `-std=`" diagnostics, so an older toolchain skips those examples instead
  of failing the whole job. `check_warnings.py` shares those rules, so an
  older compiler missing a new standard-library header is not reported as
  a warning in our source. When the runner's compiler is older than GCC 15,
  C++23/26 examples are skipped outright — the modern toolchain job checks
  them instead of reporting a false source defect.
- `check_doc_examples.py` tolerates non-UTF-8 program output and no longer
  treats a compiler-dependent "expected segfault" as stale on clang: the
  article itself says the program may also appear to work. Compiler family
  is detected by name, so `g++-14` is still GCC (an earlier string compare
  wrongly treated it as clang and broke the sanitizer job).
- GitHub Actions upgraded to `actions/checkout@v7` and
  `actions/setup-python@v7` (Dependabot issues #1 and #2), removing the
  Node 20 deprecation warnings.

### Changed

- CI shard planning now uses the script's `--json` output, and apt installs
  use the correct `--no-install-recommends` flag.
