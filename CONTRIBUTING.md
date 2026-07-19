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
├── cpp-mastery-roadmap/      # Staged learning path with projects
│   ├── 00-overview.md        # Roadmap index
│   ├── 01-beginner/          # Stage 1: Beginner projects
│   ├── 02-intermediate/      # Stage 2: Intermediate projects
│   ├── 03-advanced/          # Stage 3: Advanced projects
│   ├── 04-stage4/            # Stage 4: More practice
│   ├── ...                   # Stages 5-40
│   └── 40-stage40/           # Stage 40: C++23 features
└── tinyshell/                # Unix shell implementation
```

### Documentation Convention

- One document = one central idea
- Bilingual: English in `docs/en/`, Chinese in `docs/中文版/`
- Filename: lowercase-kebab-case, e.g., `ownership-and-lifetime.md`

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

Each project follows this structure:
```
project-name/
├── CMakeLists.txt            # Build system
├── README.md                 # English documentation
├── README.zh-CN.md           # Chinese documentation
├── include/                  # Header files
├── src/                      # Implementation files
└── tests/                    # Test files
```

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

---

## A Final Word

This project improves through dialogue.
If you see something questionable, say so.

That is the spirit in which it is written.
