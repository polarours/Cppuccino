# Changelog

Notable changes to Cppuccino. This file is the short, readable index for
readers and for stable references.

## [Unreleased]

### Added

- Community health files: [Code of Conduct](CODE_OF_CONDUCT.md),
  [Security Policy](SECURITY.md), [Support](SUPPORT.md), `CODEOWNERS`, and
  structured GitHub issue forms for bugs and feature requests.
- CI, documentation, Pages, and license badges at the top of both READMEs.
- `tools/run_all_checks.sh` now accepts combined flags, so
  `--full --sanitize` is one local command instead of two separate runs.
- `tools/fix_html_lang.py` rewrites the generated Chinese pages to
  `lang="zh-CN"`; `site-src/assets/lang.js` keeps the attribute correct during
  Material instant navigation. Every Play-mode language toggle now updates the
  document language as well as the visible text.
- Navigation now uses each article's real H1 instead of a lowercased filename,
  and groups the 70 concepts into Foundations / Modern C++ / Design Patterns /
  Architecture & Enterprise / Systems & Performance / Craft & Tooling. The 37
  pitfalls are grouped by failure family. A new top-level **Play** menu links
  Arcade, UB Camp, Proof Gate, and the Labs.
- `/lab/` is now an index as well as the Lifetime Timeline: four numbered cards
  link to the lifetime, iterator, data-race, and double-free experiments. The
  homepage label changed from `LIFETIME LAB` to `INTERACTIVE LABS`, and both
  READMEs link the Labs from the first screen.
- Every generated page now carries a short, page-specific meta description
  derived from its first paragraph. A Material `overrides/main.html` adds
  Open Graph and Twitter card tags, backed by a real 1200×630 branded social
  image. `robots.txt` points at the sitemap, and `404.html` gives lost readers
  a recovery path instead of a blank error.

### Fixed

- The English and Chinese Start Here pages had malformed template tokens
  (`{{DOCS}`), so the live pages showed literal placeholders instead of the
  repository counts. `tools/build_site.py` now fails the build if a known
  token is left unresolved.
- Documentation and Play pages reported `<html lang="en">` even when the
  reader selected Chinese. This is a WCAG language-of-page failure, not only
  an SEO detail.

## [1.0.0] - 2026-11-25

### Changed

- The dangling-pointer pitfall (EN + ZH) now embeds the Lifetime Timeline lab
  inline. It is progressive enhancement: the article is complete without
  JavaScript, and the embedded instrument follows the page's theme and
  language.
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
- Three more inline labs, each embedded in its EN + ZH pitfall article:
  `/lab/iterator/` (vector reallocation, `_GLIBCXX_DEBUG` singular-iterator
  abort), `/lab/race/` (lost update, real TSan report), and
  `/lab/double-free/` (second delete, real ASan double-free report). All three
  share the same embed/theme/language contract as the lifetime lab.
- A **UB Training Camp** at `/arcade/camp/`: a four-boss route through
  lifetime, containers, concurrency, and ownership. Each stop is
  read → reproduce → repair → prove, with links to the real pitfall article,
  runnable example, and boss fight. It reads the arcade's local progress and
  marks cleared bosses in place.
- The camp now tracks a four-step chain per boss: **READ → LAB → FIGHT →
  PROVE**. Camp article links carry `?camp=1`; the article's inline lab records
  READ when it mounts and LAB when the iframe loads, while the boss fight
  records FIGHT; PROVE is written only by passing the Proof Gate.
- A **Proof Gate** at `/arcade/camp/prove/`: three bilingual questions per boss
  (12 total), with explanations after each answer. A 3/3 pass writes
  `cppuccino-proof-v1`; the camp card only reaches PROVE / CLEARED after the
  gate is stamped. Passing all four reveals a four-stamp camp completion panel.
- Every boss page now has a **CAMP** link in the header and a **BACK TO CAMP**
  link beside the editor. After a win, the reader can return to the camp and
  see the card advance to PROVE / CLEARED without using the browser back button.
- The camp now shows a **NEXT STEP** strip derived from the same progress state:
  it points directly at the first missing READ / LAB / FIGHT / PROVE action.
  A completed camp switches it to a proof-review link.
- The arcade landing banner now shows live camp progress (`0 / 4 BOSSES`) and
  changes its CTA from **ENTER CAMP** to **CONTINUE CAMP** or **REVIEW CAMP**
  as proofs are stamped.
- A **Double Free boss fight** at `/arcade/boss/double-free/`: use the resource
  while it lives, release it once, and set the pointer to `nullptr`. A second
  `delete` triggers a real ASan `attempting double-free` report captured locally.
  This completes the four-boss UB set (lifetime / iterator / concurrency /
  ownership).
- A **Data Race boss fight** at `/arcade/boss/data-race/`: two threads execute
  your worker body; a bare shared increment triggers a real ThreadSanitizer
  report captured locally. Guarding the counter with `lock(m)` clears the boss.
  This completes the lifetime / iterator / concurrency UB trio.
- An **Iterator Invalidation boss fight** at `/arcade/boss/iterator-invalidation/`:
  the vector grows, your iterator becomes singular, and the debugger shows a
  real `-D_GLIBCXX_DEBUG` report captured locally. It appears as a Systems
  world challenge in the arcade.
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
