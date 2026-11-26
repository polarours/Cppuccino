#!/usr/bin/env python3
"""Set ``<html lang>`` correctly on the Chinese pages of a built site.

MkDocs Material exposes one global ``theme.language``; this repository has
English and Chinese pages in the same build.  The runtime ``lang.js`` keeps
the attribute correct during instant navigation, but the generated HTML should
also be correct before JavaScript runs (SEO, screen readers, no-JS readers).

Usage:
    python3 tools/fix_html_lang.py site
    python3 tools/fix_html_lang.py --check site
"""
from __future__ import annotations

import argparse
import re
from pathlib import Path
from urllib.parse import unquote

HTML_TAG = re.compile(r'<html\b[^>]*>', re.IGNORECASE)
LANG_ATTR = re.compile(r'\blang="[^"]*"', re.IGNORECASE)


def is_chinese(relative: str) -> bool:
    # MkDocs normally writes the Unicode directory name, but a URL-encoded
    # path (or a caller passing a URL-ish path) must be recognised too.
    for candidate in (relative, unquote(relative)):
        if ('/中文版/' in candidate or candidate.startswith('zh/')
                or '/zh/' in candidate):
            return True
    return False


def set_lang(text: str) -> str:
    match = HTML_TAG.search(text)
    if match is None:
        return text
    tag = match.group(0)
    if LANG_ATTR.search(tag):
        new_tag = LANG_ATTR.sub('lang="zh-CN"', tag, count=1)
    else:
        new_tag = tag[:5] + ' lang="zh-CN"' + tag[5:]
    return text[:match.start()] + new_tag + text[match.end():]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('site', nargs='?', default='site',
                        help='built site directory (default: site)')
    parser.add_argument('--check', action='store_true',
                        help='report changes instead of writing them')
    parser.add_argument('--require', type=int, default=0, metavar='N',
                        help='fail unless at least N Chinese pages are found')
    args = parser.parse_args()

    root = Path(args.site)
    if not root.is_dir():
        raise SystemExit(f'not a directory: {root}')

    changed = 0
    checked = 0
    for path in sorted(root.rglob('*.html')):
        relative = path.relative_to(root).as_posix()
        if not is_chinese(relative):
            continue
        checked += 1
        text = path.read_text(encoding='utf-8', errors='replace')
        updated = set_lang(text)
        if updated == text:
            continue
        changed += 1
        print(f'{relative}: lang -> zh-CN')
        if not args.check:
            path.write_text(updated, encoding='utf-8')

    print(f'{checked} Chinese page(s), {changed} updated')
    if args.require and checked < args.require:
        print(f'expected at least {args.require} Chinese page(s), found {checked}')
        return 1
    if args.check and changed:
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
