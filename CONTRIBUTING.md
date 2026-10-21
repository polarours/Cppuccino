# Contributing

Thank you for your interest in this project.

This repository is a personal work-in-progress.
All content reflects the author's current understanding of C++, which is
necessarily limited and evolving. Mistakes and omissions are possible.

The project exists primarily as a way to learn by writing.
If something is unclear or incorrect, pointing it out is genuinely appreciated.

---

## What to Contribute

Contributions are welcome, especially in the following forms:

- **Technical corrections** — fixing wrong statements, outdated information
- **Clarifications or simplifications** — making complex ideas easier to understand
- **Small refinements** — wording, formatting, code style consistency
- **Thoughtful discussions** — counterexamples, alternative approaches, edge cases
- **New examples** — runnable code that demonstrates a concept clearly
- **New pitfalls** — common mistakes with clear explanations and fixes

The goal is not to be exhaustive, but to be precise.

---

## Repository Structure

```
Cppuccino/
├── docs/en/                  # English concept articles
├── docs/中文版/              # Chinese concept articles
├── pitfalls/en/              # English pitfall articles
├── pitfalls/中文版/          # Chinese pitfall articles
├── examples/                 # Runnable .cpp examples
├── tools/                    # Repository-level consistency checks
├── cpp-mastery-roadmap/      # Staged learning path with projects
│   ├── 00-overview.md        # Roadmap index
│   ├── 01-beginner/          # Stage 1: Beginner projects
│   ├── 02-intermediate/      # Stage 2: Intermediate projects
│   ├── 03-advanced/          # Stage 3: Advanced projects
│   ├── 04-stage4/            # Stage 4: More practice
│   ├── ...                   # Stages 5-43
│   └── 43-stage43/           # Stage 43: CI/CD and tooling
└── tinyshell/                # Unix shell implementation
```

### Documentation Convention

- One document = one central idea
- Bilingual: English in `docs/en/`, Chinese in `docs/中文版/`
- Filename: lowercase-kebab-case, e.g., `ownership-and-lifetime.md`
- The two halves keep the same heading skeleton and the same code; only the
  prose and the printed messages are translated

### Pitfalls Convention

- Bilingual: English in `pitfalls/en/`, Chinese in `pitfalls/中文版/`
- Structure: Problem → Examples → Why It Matters → Prevention
- Cross-reference related pitfalls when possible

### Examples Convention

- One `.cpp` file per concept area
- Compilable standalone: `g++ -std=c++17 example.cpp -o example`
- Educational with intentional UB examples marked
- Filename: lowercase-kebab-case with `.cpp` extension

### Project Convention

Every project has these, without exception:

```
project-name/
├── CMakeLists.txt            # Build system
├── README.md                 # English documentation
├── README.zh-CN.md           # Chinese documentation
└── tests/                    # Test files
```

Most projects also split the sources:
```
├── include/                  # Header files
└── src/                      # Implementation files
```

A small demo project may keep its sources flat, or group them into
subdirectories of its own, as long as it stays self-contained. The
`student-management` versions, `copy-move-demo` and `producer-consumer-demo`
are flat; `key-value-store` groups its sources by concern.

- Use C++17 unless the project specifically needs C++20/23
- Tests use a simple `expect()` function (no external framework)
- Each project should have at least 3 tests

---

## Pull Requests

- Keep pull requests small and focused
- For substantial changes, open an issue first
- Ensure contributions are respectful and technically sound
- Follow the existing code style and conventions

By contributing, you confirm that your work is original or properly attributed
and that you agree to license it under the project's license.

---

## Building and Testing

Most projects use CMake:

```bash
cd cpp-mastery-roadmap/<stage>/<project>
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

For examples:
```bash
g++ -std=c++17 examples/example-name.cpp -o /tmp/example && /tmp/example
```

### Repository Checks

The scripts under `tools/` keep the repository honest. Run them before
opening a pull request that touches `docs/`, `pitfalls/` or `examples/`:

```bash
# Everything the CI runs for the documentation set.
tools/run_all_checks.sh

# Add the project builds and tests.
tools/run_all_checks.sh --full
```

The individual checks can also be run on their own:

| Command | What it catches |
|---------|-----------------|
| `python3 tools/check_docs.py` | A file in `docs/en/` with an English filename but a Chinese body; the two halves of a document drifting apart |
| `python3 tools/check_links.py` | A relative link or `#anchor` that no longer resolves |
| `python3 tools/check_doc_examples.py` | A complete example inside a document that no longer compiles or runs |
| `python3 tools/check_examples.py` | A standalone file in `examples/` that no longer compiles, or a header that no longer describes how to build it |
| `python3 tools/check_stats.py` | A statistic in the READMEs that no longer matches the repository |
| `python3 tools/build_all_projects.py` | A project that fails to configure, build or pass its tests |

The first script is what catches a file that sits in `docs/en/` with an
English filename but a Chinese body. The second one catches an example that
no longer compiles. Both are backed by the
[bilingual-doc-checker](cpp-mastery-roadmap/39-stage39/bilingual-doc-checker/)
project.

### Continuous Integration

The same scripts run in CI, so a green local run means a green CI run:

- `.github/workflows/docs.yml` — the documentation checks, on every push
- `.github/workflows/build.yml` — every project, split over ten shards that
  are balanced by project count, plus the tinyshell submodule

The shards are planned by `tools/plan_ci_shards.py`, so adding a project does
not need a workflow edit.

To make the checks binding, mark the following as required status checks in
the repository settings under **Settings → Branches → Branch protection
rules**:

| Check | Job |
|-------|-----|
| `Documentation checks` | `docs` in the *Documentation* workflow |
| `shard N` | `build` in the *Build* workflow, for every shard |
| `tinyshell` | `tinyshell` in the *Build* workflow |

The job names are fixed by the workflow files, so they can be typed in as
they are.

---

## A Final Word

This project improves through dialogue.
If you see something questionable, say so.

That is the spirit in which it is written.
