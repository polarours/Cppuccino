# Pull Request

## What changed

<!-- One or two sentences. What is different after this pull request? -->

## Why

<!--
The reason the change is needed, not a restatement of the diff.
Link the issue or the documentation gap it closes.
-->

## Type of change

- [ ] New documentation
- [ ] Documentation translation (English <-> Chinese)
- [ ] New project or example
- [ ] Fix to an existing document
- [ ] Fix to a project or example
- [ ] Repository tooling (scripts, CI)

## Checklist

The repository checks run in CI. Run them locally first with:

```bash
tools/run_all_checks.sh
```

- [ ] `tools/run_all_checks.sh` passes
- [ ] The English and Chinese halves of any document I touched still agree
      (heading skeleton and code)
- [ ] Every code block I added or changed compiles
- [ ] Relative links and anchors still resolve
- [ ] A new project has `CMakeLists.txt`, `README.md`, `README.zh-CN.md`
      and tests

## Notes for the reviewer

<!--
Anything that needs a second look: a place where the English and the Chinese
wording deliberately differ, a code block that is an illustrative fragment
rather than a complete program, a trade-off you were not sure about.
-->
