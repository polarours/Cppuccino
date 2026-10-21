# Repository Checks

Scripts that keep the repository honest. They run in CI on every push, and
they are the same scripts a contributor runs locally, so a green local run
means a green CI run.

```bash
tools/run_all_checks.sh          # the documentation checks
tools/run_all_checks.sh --full   # and every project build and test
```

## The checks

| Script | What it catches |
|--------|-----------------|
| `check_docs.py` | A file in `docs/en/` with an English filename but a Chinese body; the English and Chinese halves of a document drifting apart |
| `check_links.py` | A relative link or `#anchor` that no longer resolves |
| `check_doc_examples.py` | A complete example inside a document that no longer compiles or runs |
| `check_examples.py` | A standalone file in `examples/` that no longer compiles, or a header that no longer describes how to build it |
| `check_stats.py` | A statistic in the READMEs that no longer matches the repository |
| `build_all_projects.py` | A project that fails to configure, build or pass its tests |

## How the language check works

The obvious way to ask "is this document Chinese?" is to count CJK
characters against latin ones. That does not work here. An API reference is
mostly table rows like:

```markdown
| `save` | `bool save(const std::string& filename) const` | 保存到文件 |
```

The identifiers are latin, the description is Chinese, and the ratio says
"mostly English" even though the prose is Chinese.

So the check works per line. A line is Chinese-dominant when it holds at
least two CJK characters and at least as many CJK characters as latin
*words*. A line without any CJK carries no signal and is not counted either
way. Fenced code blocks are removed before counting, so a comment written in
the wrong language never influences the verdict.

## How the pairs are matched

`docs/en/` and `docs/中文版/` share no filename convention, so the two
halves of a document are matched by their structure: the ordered list of
code blocks, compared token by token. String literal contents are blanked
out, because a translated document prints its messages in its own language
while running the very same program.

The pitfall articles are matched by hand instead, and listed in
`pitfall_pairs.tsv`. Their Chinese filenames are translations
(`悬垂指针` for `dangling-pointer`), so there is no shared spelling to
match on, and a structural fallback pairs the wrong files when two articles
happen to use similar examples.

The matching is backed by the
[bilingual-doc-checker](../cpp-mastery-roadmap/39-stage39/bilingual-doc-checker/)
project, a C++ library of the same rules.

## The compiler

The checkers use `g++` by default and honour `CXX`, so a job can point at a
newer toolchain:

```bash
CXX=g++-14 tools/check_examples.py
```

An example that needs a standard the compiler does not know is reported as
skipped rather than as broken, so a runner with an older compiler does not
fail over it.
