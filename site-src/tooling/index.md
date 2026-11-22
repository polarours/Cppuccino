# Tooling

The site is built from the repository itself by
[`tools/build_site.py`](https://github.com/polarours/Cppuccino/blob/master/tools/build_site.py).
No article is copied into a second content tree.

```bash
python3 tools/build_site.py
mkdocs build -f .site/mkdocs.yml --strict --site-dir site
```

The same checks CI runs are documented in
[`tools/README.md`](https://github.com/polarours/Cppuccino/blob/master/tools/README.md)
and [中文版](https://github.com/polarours/Cppuccino/blob/master/tools/README.zh-CN.md).
Notable changes live in the
[changelog](https://github.com/polarours/Cppuccino/blob/master/CHANGELOG.md).

## One-click environment

[GitHub Codespaces](https://codespaces.new/polarours/Cppuccino?quickstart=1)
uses the repository's [devcontainer](https://github.com/polarours/Cppuccino/blob/master/.devcontainer/devcontainer.json),
which already includes the Python dependencies needed to build this site.

## Quality gates

- `tools/check_links.py` — no broken relative links or anchors
- `tools/check_stats.py` — README numbers match the repository
- `tools/run_all_checks.sh` — the full documentation and demo suite
- `mkdocs build --strict` — the site itself has no broken links
