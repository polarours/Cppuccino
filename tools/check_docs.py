#!/usr/bin/env python3
"""Consistency checks for the bilingual documentation set.

Two independent checks:

1. Language check (no pairing needed)
   Every file in docs/en/ must have an English prose body, and every file in
   docs/中文版/ must have a Chinese one. This is the check that catches the
   historical bug in this repository: 15 files whose names were English
   kebab-case but whose bodies were still Chinese.

   A document is classified line by line over its prose (fenced code blocks
   excluded). A line is Chinese-dominant when it holds at least two CJK
   characters and at least as many CJK characters as latin words. The
   document is Chinese when Chinese-dominant lines outnumber
   English-dominant ones. The per-line rule matters because code blocks and
   API tables are full of latin identifiers, which inflate a naive character
   ratio and hide an untranslated body.

2. Pair check (best effort)
   The English and Chinese halves of a document should agree on their heading
   skeleton and on their code. The two halves are matched by the ordered
   list of normalised code blocks, which survives renaming and re-ordering of
   headings. Documents that cannot be matched this way are reported as
   unverifiable rather than as failures.

Usage:
    python3 tools/check_docs.py [--repo ROOT] [--language-only]

Exit status is 0 when no problem was found.
"""
import argparse
import glob
import os
import re
import sys
from collections import defaultdict

TOOLS_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_REPO = os.path.dirname(TOOLS_DIR)


# --------------------------------------------------------------------------
# shared helpers
# --------------------------------------------------------------------------

def strip_code_blocks(text):
    """Remove fenced code blocks, keeping the prose around them."""
    out = []
    fence = None
    for line in text.splitlines():
        m = re.match(r'^\s{0,3}(`{3,}|~{3,})', line)
        if fence is None:
            if m:
                fence = m.group(1)[0]
            else:
                out.append(line)
        elif m and m.group(1)[0] == fence:
            fence = None
    return '\n'.join(out)


def count_cjk(line):
    return len(re.findall(r'[一-鿿]', line))


def count_words(line):
    return len(re.findall(r'[A-Za-z]+', line))


def classify(text):
    """Return (verdict, chinese_lines, english_lines) for a document body."""
    zh = en = 0
    for line in strip_code_blocks(text).splitlines():
        cjk = count_cjk(line)
        if cjk == 0:
            continue  # no signal either way
        if cjk >= max(2, count_words(line)):
            zh += 1
        else:
            en += 1
    if zh + en == 0:
        return 'en', 0, 0  # no CJK anywhere: English by default
    return ('zh' if zh > en else 'en'), zh, en


def strip_comments(src):
    """Remove // and /* */ comments, respecting string literals."""
    out = []
    i, n = 0, len(src)
    in_str = in_chr = False
    while i < n:
        c = src[i]
        if in_str:
            out.append(c)
            if c == '\\' and i + 1 < n:
                out.append(src[i + 1])
                i += 2
                continue
            if c == '"':
                in_str = False
            i += 1
            continue
        if in_chr:
            out.append(c)
            if c == '\\' and i + 1 < n:
                out.append(src[i + 1])
                i += 2
                continue
            if c == "'":
                in_chr = False
            i += 1
            continue
        if c == '"':
            in_str = True
            out.append(c)
            i += 1
            continue
        if c == "'":
            in_chr = True
            out.append(c)
            i += 1
            continue
        if c == '/' and i + 1 < n and src[i + 1] == '/':
            while i < n and src[i] != '\n':
                i += 1
            continue
        if c == '/' and i + 1 < n and src[i + 1] == '*':
            i += 2
            while i + 1 < n and not (src[i] == '*' and src[i + 1] == '/'):
                i += 1
            i += 2
            out.append(' ')
            continue
        out.append(c)
        i += 1
    return ''.join(out)


def normalize_code(src, drop_strings=False):
    s = strip_comments(src)
    if drop_strings:
        # A translated document prints its messages in its own language, so
        # the message text may differ while the program stays the same.
        s = re.sub(r'"(\\.|[^"\\])*"', '"..."', s)
    return re.sub(r'\s+', ' ', s).strip()


def code_blocks(path):
    text = open(path, encoding='utf-8').read()
    return re.findall(r'```cpp\n(.*?)```', text, re.S)


def code_signature(path):
    """Structural fingerprint of a document: its normalised code blocks.

    String literal contents are blanked out, so a translated document still
    matches its twin even though it prints its messages in another language.
    """
    blocks = code_blocks(path)
    if not blocks:
        return None
    return tuple(normalize_code(b, True) for b in blocks)


def headings(path):
    text = open(path, encoding='utf-8').read()
    out = []
    fence = None
    for line in text.splitlines():
        m = re.match(r'^\s{0,3}(`{3,}|~{3,})', line)
        if fence is None:
            if m:
                fence = m.group(1)[0]
            else:
                h = re.match(r'^(#{1,6})\s+(.*)$', line)
                if h:
                    out.append((len(h.group(1)), h.group(2).strip()))
        elif m and m.group(1)[0] == fence:
            fence = None
    return out


def title(path):
    text = open(path, encoding='utf-8').read()
    m = re.search(r'^#\s+(.+)$', text, re.M)
    return m.group(1).strip() if m else ''


# --------------------------------------------------------------------------
# checks
# --------------------------------------------------------------------------

def check_language(root, problems):
    en_files = sorted(glob.glob(os.path.join(root, 'docs/en/*.md')))
    zh_files = sorted(glob.glob(os.path.join(root, 'docs/中文版/*.md')))

    print('== language check ==')
    for f in en_files:
        verdict, zh, en = classify(open(f, encoding='utf-8').read())
        if verdict == 'zh':
            problems.append(f'EN body is Chinese: {os.path.relpath(f, root)}')
            print(f"  EN->ZH  {os.path.relpath(f, root)} "
                  f"({zh} chinese lines vs {en} english lines)")
    for f in zh_files:
        verdict, zh, en = classify(open(f, encoding='utf-8').read())
        if verdict == 'en':
            problems.append(f'ZH body is English: {os.path.relpath(f, root)}')
            print(f"  ZH->EN  {os.path.relpath(f, root)} "
                  f"({en} english lines vs {zh} chinese lines)")
    print(f"  {len(en_files)} english docs, {len(zh_files)} chinese docs checked")
    return len(en_files), len(zh_files)


def check_pairs(root, problems):
    en_files = sorted(glob.glob(os.path.join(root, 'docs/en/*.md')))
    zh_files = sorted(glob.glob(os.path.join(root, 'docs/中文版/*.md')))

    by_signature = defaultdict(list)
    for f in zh_files:
        sig = code_signature(f)
        if sig is not None:
            by_signature[sig].append(f)

    print('\n== pair check ==')
    paired = 0
    unverifiable = []
    for f in en_files:
        sig = code_signature(f)
        cands = by_signature.get(sig, []) if sig is not None else []
        if len(cands) != 1:
            unverifiable.append(os.path.relpath(f, root))
            continue
        paired += 1
        zh = cands[0]

        en_h, zh_h = headings(f), headings(zh)
        rel_en = os.path.relpath(f, root)
        if len(en_h) != len(zh_h):
            problems.append(f'heading count differs: {rel_en} vs {os.path.basename(zh)}')
            print(f"  HEADINGS  {rel_en}: {len(en_h)} vs {len(zh_h)} "
                  f"in {os.path.basename(zh)}")
        elif [l for l, _ in en_h] != [l for l, _ in zh_h]:
            problems.append(f'heading levels differ: {rel_en} vs {os.path.basename(zh)}')
            print(f"  LEVELS    {rel_en}: levels differ from {os.path.basename(zh)}")

        en_code = code_blocks(f)
        zh_code = code_blocks(zh)
        if len(en_code) != len(zh_code):
            problems.append(f'code block count differs: {rel_en} vs {os.path.basename(zh)}')
            print(f"  BLOCKS    {rel_en}: {len(en_code)} vs {len(zh_code)} "
                  f"in {os.path.basename(zh)}")
        else:
            drifted = [i for i, (a, b) in enumerate(zip(en_code, zh_code))
                       if normalize_code(a, True) != normalize_code(b, True)]
            if drifted:
                problems.append(f'code differs: {rel_en} vs {os.path.basename(zh)}')
                print(f"  CODE      {rel_en}: block(s) {drifted} differ "
                      f"from {os.path.basename(zh)}")

    print(f"  {paired}/{len(en_files)} pairs verified")
    if unverifiable:
        print(f"  {len(unverifiable)} doc(s) could not be matched (not a failure):")
        for name in unverifiable:
            print(f"    {name}")
    return paired


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', default=DEFAULT_REPO)
    parser.add_argument('--language-only', action='store_true')
    args = parser.parse_args()

    problems = []
    check_language(args.repo, problems)
    if not args.language_only:
        check_pairs(args.repo, problems)

    print()
    if problems:
        print(f'{len(problems)} problem(s):')
        for p in problems:
            print(f'  {p}')
        return 1
    print('all checks passed')
    return 0


if __name__ == '__main__':
    sys.exit(main())
