#!/usr/bin/env python3
"""Check that every article points at something the reader can run.

An article is satisfied when it links to a file under examples/, or to a
CMakeLists.txt of one of the projects, or when it carries a complete
program of its own. The two halves of an article have to be satisfied
the same way, and to name the same examples: a reader who follows the
link from one of them has to land on the same program.

The glossary is the one document that is satisfied by nothing: it is a
list of words, and a program would not say more about a term than the
term does.

The pitfall articles are paired by hand, because their Chinese file names
are translations. The documents under docs/ are paired by the structure
of their code blocks, which is what the bilingual checker does.

Usage: python3 tools/check_article_examples.py
"""
import glob
import os
import re
import sys
from collections import defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check_docs  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PAIRS = os.path.join(REPO, 'tools', 'pitfall_pairs.tsv')
DOCS_EN = os.path.join(REPO, 'docs', 'en')
DOCS_ZH = os.path.join(REPO, 'docs', '中文版')
PITFALLS_EN = os.path.join(REPO, 'pitfalls', 'en')
PITFALLS_ZH = os.path.join(REPO, 'pitfalls', '中文版')

# The documents that are satisfied by nothing.
EXEMPT = {'glossary.md', '术语表.md'}

LINK_RE = re.compile(r'\]\(([^)]+)\)')


def examples_of(path):
    """The examples a document names, and whether it carries its own program."""
    with open(path, encoding='utf-8', errors='replace') as handle:
        text = handle.read()
    names = []
    for link in LINK_RE.findall(text):
        raw = link.split('#', 1)[0]
        if not raw or raw.startswith(('http://', 'https://')):
            continue
        if 'examples/' not in raw and not raw.endswith('CMakeLists.txt'):
            continue
        resolved = os.path.normpath(os.path.join(os.path.dirname(path), raw))
        if os.path.exists(resolved):
            names.append(os.path.basename(resolved))
    has_program = 'int main' in text
    return sorted(set(names)), has_program


def pitfall_pairs():
    out = []
    with open(PAIRS, encoding='utf-8') as handle:
        for line in handle:
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            english, chinese = line.split('\t')
            out.append((os.path.join(PITFALLS_EN, english.strip()),
                        os.path.join(PITFALLS_ZH, chinese.strip())))
    return out


def document_pairs():
    """The documents under docs/, paired the way the bilingual checker pairs them."""
    en_files = sorted(glob.glob(os.path.join(DOCS_EN, '*.md')))
    zh_files = sorted(glob.glob(os.path.join(DOCS_ZH, '*.md')))

    by_signature = defaultdict(list)
    for f in zh_files:
        signature = check_docs.code_signature(f)
        if signature is not None:
            by_signature[signature].append(f)

    by_headings = defaultdict(list)
    by_count = defaultdict(list)
    by_english_title = defaultdict(list)
    for f in zh_files:
        by_headings[tuple(level for level, _ in check_docs.headings(f))].append(f)
        by_count[len(check_docs.headings(f))].append(f)
        gloss = check_docs.english_gloss(check_docs.headings(f))
        if gloss:
            by_english_title[gloss].append(f)

    pairs = []
    for f in en_files:
        signature = check_docs.code_signature(f)
        candidates = by_signature.get(signature, []) if signature is not None else []
        if len(candidates) != 1:
            en_h = check_docs.headings(f)
            english = re.sub(r'[^a-z0-9]+', '',
                             en_h[0][1].lower()) if en_h else None
            for key in (english, tuple(level for level, _ in en_h), len(en_h)):
                if key is None:
                    continue
                fallback = (by_english_title.get(key, []) if isinstance(key, str)
                            else by_headings.get(key, []) if isinstance(key, tuple)
                            else by_count.get(key, []))
                if len(fallback) == 1:
                    candidates = fallback
                    break
        if len(candidates) == 1:
            pairs.append((f, candidates[0]))
    return pairs


def main():
    problems = []
    checked = 0

    print('== article examples ==')
    for label, pairs in (('pitfall', pitfall_pairs()),
                         ('doc', document_pairs())):
        for english, chinese in pairs:
            if not os.path.isfile(english) or not os.path.isfile(chinese):
                problems.append(f'{label}: {english} or its twin is not on disk')
                continue
            checked += 1
            rel_en = os.path.relpath(english, REPO)
            rel_zh = os.path.relpath(chinese, REPO)

            if os.path.basename(english) in EXEMPT:
                continue

            en_names, en_program = examples_of(english)
            zh_names, zh_program = examples_of(chinese)

            if not en_names and not en_program:
                problems.append(f'{rel_en}: names no example and has no '
                                f'program of its own')
            if not zh_names and not zh_program:
                problems.append(f'{rel_zh}: names no example and has no '
                                f'program of its own')
            if en_names and zh_names and en_names != zh_names:
                problems.append(f'{rel_en}: the two halves name different '
                                f'examples ({en_names} and {zh_names})')

    print(f'  {checked} article pair(s) checked')
    if problems:
        print(f'\n{len(problems)} problem(s):')
        for problem in problems:
            print(f'  {problem}')
        return 1
    print('\nevery article points at something the reader can run')
    return 0


if __name__ == '__main__':
    sys.exit(main())
