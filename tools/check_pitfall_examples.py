#!/usr/bin/env python3
"""Check that every pitfall article points at a runnable example.

The pitfall articles are the ones that show what a mistake looks like, and
an example that can be run is the fastest way to see it. Each article is
expected to name one, in both language halves, and the file it names has to
exist.

The English and the Chinese half must name the same example: a reader who
follows the link from one of them has to land on the same program.

Usage: python3 tools/check_pitfall_examples.py
"""
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PAIRS = os.path.join(REPO, 'tools', 'pitfall_pairs.tsv')
EXAMPLES = os.path.join(REPO, 'examples')

LINK_RE = re.compile(r'\]\(\.\./\.\./examples/(pitfall-[a-z0-9-]+\.cpp)\)')


def pairs():
    """The (english, chinese) file names of the pitfall articles."""
    out = []
    with open(PAIRS, encoding='utf-8') as handle:
        for line in handle:
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            parts = line.split('\t')
            if len(parts) != 2:
                print(f'  MALFORMED  {line}')
                continue
            out.append((parts[0].strip(), parts[1].strip()))
    return out


def links(path):
    """The examples a file links to."""
    with open(path, encoding='utf-8', errors='replace') as handle:
        return LINK_RE.findall(handle.read())


def main():
    problems = []
    checked = 0

    print('== pitfall examples ==')
    for english, chinese in pairs():
        en_path = os.path.join(REPO, 'pitfalls', 'en', english)
        zh_path = os.path.join(REPO, 'pitfalls', '中文版', chinese)
        if not os.path.isfile(en_path) or not os.path.isfile(zh_path):
            problems.append(f'{english}: the pair is not on disk')
            continue
        checked += 1
        en_links = links(en_path)
        zh_links = links(zh_path)

        if not en_links:
            problems.append(f'pitfalls/en/{english}: no example is linked')
        if not zh_links:
            problems.append(f'pitfalls/中文版/{chinese}: no example is linked')
        if en_links and zh_links and en_links != zh_links:
            problems.append(f'{english}: the two halves name different examples '
                            f'({en_links} and {zh_links})')
        for name in set(en_links) | set(zh_links):
            if not os.path.isfile(os.path.join(EXAMPLES, name)):
                problems.append(f'{english}: {name} does not exist')

    print(f'  {checked} article pair(s) checked')
    if problems:
        print(f'\n{len(problems)} problem(s):')
        for problem in problems:
            print(f'  {problem}')
        return 1
    print('\nevery pitfall article names an example that exists')
    return 0


if __name__ == '__main__':
    sys.exit(main())
