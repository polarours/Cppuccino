#!/usr/bin/env python3
"""Check that bilingual docs and UI strings use the preferred terminology.

The canonical table lives in docs/i18n/terminology.md. Only the "禁止写法"
column is enforced; the glossary file itself is skipped so it can document
the forbidden forms.

Usage: python3 tools/check_terminology.py [repo_root]
"""
import argparse
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GLOSSARY = os.path.join(ROOT, 'docs', 'i18n', 'terminology.md')

SCAN_GLOBS = (
    'docs/中文版/*.md',
    'pitfalls/中文版/*.md',
    'examples/README.zh-CN.md',
    'site-src/**/*.md',
    'site-src/**/*.html',
    'site-src/**/*.js',
)


def load_terms():
    """Return {forbidden_variant: preferred_term} from the glossary table."""
    if not os.path.isfile(GLOSSARY):
        raise SystemExit(f'glossary not found: {GLOSSARY}')

    terms = {}
    with open(GLOSSARY, encoding='utf-8') as handle:
        for line in handle:
            line = line.strip()
            if not line.startswith('|'):
                continue
            cells = [cell.strip() for cell in line.strip('|').split('|')]
            if len(cells) < 3:
                continue
            if cells[0].startswith(':') or set(cells[0]) <= set(':- '):
                continue
            if cells[0] == 'English / 概念':
                continue
            preferred = cells[1]
            for variant in re.split(r'[;；,，]', cells[2]):
                variant = variant.strip()
                if variant:
                    terms[variant] = preferred
    return terms


def files_to_scan(root):
    found = set()
    for pattern in SCAN_GLOBS:
        for path in glob.glob(os.path.join(root, pattern), recursive=True):
            if os.path.abspath(path) == os.path.abspath(GLOSSARY):
                continue
            if os.path.isfile(path):
                found.add(path)
    return sorted(found)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', nargs='?', default=ROOT)
    args = parser.parse_args()
    root = os.path.abspath(args.root)

    terms = load_terms()
    problems = []
    files = files_to_scan(root)

    for path in files:
        with open(path, encoding='utf-8', errors='replace') as handle:
            for number, line in enumerate(handle, 1):
                for variant, preferred in terms.items():
                    if variant in line:
                        problems.append(
                            (os.path.relpath(path, root), number,
                             variant, preferred))

    print(f'checked {len(files)} file(s) against {len(terms)} forbidden variant(s)')
    if problems:
        print(f'\n{len(problems)} terminology mismatch(es):')
        for path, number, variant, preferred in problems[:80]:
            print(f'  {path}:{number}: "{variant}" -> "{preferred}"')
        if len(problems) > 80:
            print(f'  ... and {len(problems) - 80} more')
        return 1
    print('terminology is consistent')
    return 0


if __name__ == '__main__':
    sys.exit(main())
