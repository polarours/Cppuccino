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
| `check_docs.py` | A file in `docs/en/` with an English filename but a Chinese body; the English and Chinese halves of a document drifting apart; a code block that was cut short, so its braces do not balance |
| `check_links.py` | A relative link or `#anchor` that no longer resolves |
| `check_doc_examples.py` | A complete example inside a document that no longer compiles or runs, or that reads freed memory |
| `check_examples.py` | A standalone file in `examples/` that no longer compiles, a header that no longer describes how to build it, two examples writing to the same binary, a body that is still in Chinese, or an example that leaks |
| `check_stats.py` | A statistic in the READMEs that no longer matches the repository |
| `check_project_readmes.py` | A project README that drifted from its twin, that names a binary the project does not build, or that is written in the wrong language |
| `check_readme_examples.py` | A C++ example in a project README that no longer compiles |
| `check_warnings.py` | A warning the compiler found in a project or an example, other than the ones that are on purpose |
| `check_pitfall_examples.py` | A pitfall article that does not name an example, or one that names an example that is not there |
| `check_overview_parity.py` | The two roadmap indexes listing different stages or different projects |
| `build_all_projects.py` | A project that fails to configure, build or pass its tests |
| `run_sanitizers.py` | A test that reads freed memory, writes out of bounds, or leaks, and still passes |

The last two run the projects, the rest read the repository:

```bash
python3 tools/build_all_projects.py --jobs 4
python3 tools/run_sanitizers.py --jobs 4
```

`run_sanitizers.py` builds every project a second time with
`-fsanitize=address,undefined` and reports a leak or a use-after-free that
the ordinary build waves through. It found three real defects in this
repository: a control block that was freed while it was still in use, a
list that never released its nodes, and a pair of objects that owned each
other.

The same script answers a different question with `--tsan`:

```bash
python3 tools/run_sanitizers.py --tsan --jobs 4
```

ThreadSanitizer looks for two threads touching the same memory without a
lock. It found a descriptor that was closed while another thread was still
reading it, a value that was moved out of a node before the exchange that
owned it, and a timer thread that outlived the object it belonged to.

The two example checkers answer the same question about the examples:

```bash
python3 tools/check_examples.py --sanitize --jobs 4
python3 tools/check_doc_examples.py --sanitize
```

The examples are teaching material, so a leak in one teaches a leak. The
sanitizers found a signed integer overflow in a loop that sums a million
numbers into an `int`, and a chat room that owned its users while every
user owned the chat room.

The second checker also runs the pitfall articles, and two of them show a
mistake the program does not survive: a dangling reference and an iterator
that an insert has already invalidated. Those are listed in
`tools/known_failures.tsv`, and a file that starts failing in a different
way is still reported.

The compiler's own warnings are checked as well:

```bash
python3 tools/check_warnings.py --jobs 4
```

A warning is a mistake the compiler has already found. Three of the
examples break the rules on purpose -- they are the ones that show what a
dangling reference or a slice looks like -- and they are listed in
`tools/known_warnings.tsv`. Everything else is expected to be quiet.


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
