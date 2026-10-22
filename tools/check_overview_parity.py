#!/usr/bin/env python3
"""Check that the two roadmap indexes describe the same thing.

`cpp-mastery-roadmap/00-overview.md` and its Chinese twin are the index of
the whole repository. They have drifted before: the Chinese one stopped
after stage 4 and listed 2 of the 151 projects.

This compares their heading skeletons and the project links they carry, so
the two indexes cannot fall apart again.

Usage: python3 tools/check_overview_parity.py [repo_root]
"""
import argparse
import glob
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = 'cpp-mastery-roadmap'


def headings(text):
    out = []
    fence = None
    for line in text.splitlines():
        m = re.match(r'^\s{0,3}(`{3,}|~{3,})', line)
        if fence is None:
            if m:
                fence = m.group(1)[0]
                continue
            h = re.match(r'^(#{1,4})\s+', line)
            if h:
                out.append(len(h.group(1)))
        elif m and m.group(1)[0] == fence:
            fence = None
    return out


def project_links(text):
    """The project README paths the index links to.

    A stage-level README is left out: each language links to its own stage
    overview, so those two are expected to differ.
    """
    links = set(re.findall(r'\]\(([^)]*?/README\.md)\)', text))
    return {l for l in links if l.count('/') >= 2}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', nargs='?', default=REPO)
    args = parser.parse_args()
    root = os.path.abspath(args.root)

    en_path = os.path.join(root, ROADMAP, '00-overview.md')
    zh_path = os.path.join(root, ROADMAP, '00-overview.zh-CN.md')
    if not (os.path.isfile(en_path) and os.path.isfile(zh_path)):
        print('the roadmap index is missing one of its two halves')
        return 1

    en = open(en_path, encoding='utf-8').read()
    zh = open(zh_path, encoding='utf-8').read()

    en_h, zh_h = headings(en), headings(zh)
    en_links, zh_links = project_links(en), project_links(zh)

    print('== roadmap index ==')
    print(f'  headings:   {len(en_h)} en, {len(zh_h)} zh')
    print(f'  projects:   {len(en_links)} en, {len(zh_links)} zh')

    problems = []

    # The index is the way into the repository: every article and every
    # pitfall has to be reachable from it.
    for kind, directory, pattern in (
            ('article', 'docs', r'\.\./docs/(?:en|中文版)/([^)]+\.md)'),
            ('pitfall', 'pitfalls', r'\.\./pitfalls/(?:en|中文版)/([^)]+\.md)')):
        for name, text in (('en', en), ('zh', zh)):
            linked = set(re.findall(pattern, text))
            folder = '中文版' if name == 'zh' else 'en'
            on_disk = {os.path.basename(f)
                       for f in glob.glob(os.path.join(root, directory, folder,
                                                       '*.md'))}
            missing = sorted(on_disk - linked)
            if missing:
                problems.append(
                    f'the {name} index does not link {len(missing)} {kind}(s)')
                for m in missing[:5]:
                    problems.append(f'    {m}')

    if en_h != zh_h:
        problems.append('the two indexes have different heading skeletons')
        for index in range(min(len(en_h), len(zh_h))):
            if en_h[index] != zh_h[index]:
                problems.append(
                    f'  heading {index + 1} is level {en_h[index]} in English '
                    f'but level {zh_h[index]} in Chinese')
        if len(en_h) != len(zh_h):
            problems.append(
                f'  the English index has {len(en_h)} headings, the Chinese '
                f'one {len(zh_h)}')
    missing = sorted(en_links - zh_links)
    if missing:
        problems.append(f'  the Chinese index does not list {len(missing)} '
                        f'project(s)')
        for path in missing[:10]:
            problems.append(f'    {path}')
        if len(missing) > 10:
            problems.append(f'    ... and {len(missing) - 10} more')

    if problems:
        print(f'\n{len(problems)} problem(s):')
        for p in problems:
            print(f'  {p}' if not p.startswith('  ') else p)
        return 1
    print('\nthe two indexes describe the same thing')
    return 0


if __name__ == '__main__':
    sys.exit(main())
