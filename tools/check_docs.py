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
    last_was_digit = False
    while i < n:
        c = src[i]
        if in_str:
            out.append(c)
            last_was_digit = c.isdigit()
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
            last_was_digit = c.isdigit()
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
            last_was_digit = False
            i += 1
            continue
        if c == "'":
            # A C++14 digit separator, as in 1'000'000, is not a character
            # literal. Treating it as one would swallow the rest of the
            # file, comments included.
            if last_was_digit and i + 1 < n and src[i + 1].isdigit():
                out.append(c)
                last_was_digit = False
                i += 1
                continue
            in_chr = True
            out.append(c)
            last_was_digit = False
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
            last_was_digit = False
            continue
        out.append(c)
        last_was_digit = c.isdigit()
        i += 1
    return ''.join(out)


MULTI_CHAR_OPS = (
    '<<=', '>>=', '->*', '...', '::', '->', '++', '--', '<<', '>>',
    '<=', '>=', '==', '!=', '&&', '||', '+=', '-=', '*=', '/=', '%=',
    '&=', '|=', '^=', '.*',
)


def tokenize(src):
    """Split C++ source into tokens.

    Whitespace is dropped, so "template<typename T>" and "template
    <typename T>" produce the same token sequence. Multi-character operators
    are kept together so that "<<" never looks like two "<".
    """
    tokens = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c.isspace():
            i += 1
            continue
        if c.isalpha() or c == '_':
            start = i
            while i < n and (src[i].isalnum() or src[i] == '_'):
                i += 1
            tokens.append(src[start:i])
            continue
        if c.isdigit():
            start = i
            while i < n:
                if src[i].isdigit() or src[i] == '.':
                    i += 1
                elif src[i] == "'" and i + 1 < n and src[i + 1].isdigit():
                    # A C++14 digit separator, as in 1'000'000. It must not
                    # be mistaken for the start of a character literal.
                    i += 1
                elif src[i].isalpha() or src[i] == '_':
                    i += 1
                else:
                    break
            tokens.append(src[start:i])
            continue
        if c in '"\'':
            quote = c
            token = quote
            i += 1
            while i < n:
                if src[i] == '\\' and i + 1 < n:
                    token += src[i:i + 2]
                    i += 2
                    continue
                token += src[i]
                if src[i] == quote:
                    i += 1
                    break
                i += 1
            tokens.append(token)
            continue
        for op in MULTI_CHAR_OPS:
            if src.startswith(op, i):
                tokens.append(op)
                i += len(op)
                break
        else:
            tokens.append(c)
            i += 1
    return tokens


def normalize_code(src, drop_strings=False):
    """Token sequence of a code block, as the comparison actually uses it."""
    s = strip_comments(src)
    if drop_strings:
        # A translated document prints its messages in its own language, so
        # the message text may differ while the program stays the same.
        s = re.sub(r'"(\\.|[^"\\])*"', '""', s)
    return tokenize(s)


def code_blocks(path):
    text = open(path, encoding='utf-8').read()
    return re.findall(r'```cpp\n(.*?)```', text, re.S)


def unbalanced(src):
    """The leftover brace/paren/bracket depth of a code block.

    A block that does not balance was cut short, which is easy to do when an
    example is edited and is worth reporting. String and character literals
    are respected, and a C++14 digit separator is not mistaken for the start
    of a character literal.
    """
    depth = {'{': 0, '(': 0, '[': 0}
    closing = {'}': '{', ')': '(', ']': '['}
    i, n = 0, len(src)
    in_string = in_char = in_line = in_block = False
    last_digit = False
    while i < n:
        c = src[i]
        if in_line:
            in_line = c != '\n'
            i += 1
            continue
        if in_block:
            if c == '*' and i + 1 < n and src[i + 1] == '/':
                in_block = False
                i += 2
                continue
            i += 1
            continue
        if in_string:
            if c == '\\':
                i += 2
                continue
            in_string = c != '"'
            i += 1
            continue
        if in_char:
            if c == '\\':
                i += 2
                continue
            in_char = c != "'"
            i += 1
            continue
        if c == '/' and i + 1 < n and src[i + 1] == '/':
            in_line = True
            i += 2
            continue
        if c == '/' and i + 1 < n and src[i + 1] == '*':
            in_block = True
            i += 2
            continue
        if c == '"':
            in_string = True
            last_digit = False
            i += 1
            continue
        if c == "'":
            if not (last_digit and i + 1 < n and src[i + 1].isdigit()):
                in_char = True
            last_digit = False
            i += 1
            continue
        if c in depth:
            depth[c] += 1
        elif c in closing:
            depth[closing[c]] -= 1
        last_digit = c.isdigit()
        i += 1
    return depth


def code_signature(path):
    """Structural fingerprint of a document: its tokenised code blocks.

    String literal contents are blanked out, so a translated document still
    matches its twin even though it prints its messages in another language.
    """
    blocks = code_blocks(path)
    if not blocks:
        return None
    return tuple(tuple(normalize_code(b, True)) for b in blocks)


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


def load_pair_map(root, relative_path):
    """Read an "english<TAB>chinese" mapping file.

    Returns a dict from the English file name to the Chinese file name, or
    an empty dict when the file does not exist.
    """
    path = os.path.join(root, relative_path)
    if not os.path.isfile(path):
        return {}
    pairs = {}
    for line in open(path, encoding='utf-8'):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        parts = line.split('\t')
        if len(parts) != 2:
            parts = line.split()
        if len(parts) == 2:
            pairs[parts[0].strip()] = parts[1].strip()
    return pairs


# --------------------------------------------------------------------------
# checks
# --------------------------------------------------------------------------

def check_language(root, problems):
    en_files = sorted(glob.glob(os.path.join(root, 'docs/en/*.md')))
    zh_files = sorted(glob.glob(os.path.join(root, 'docs/中文版/*.md')))
    pit_en = sorted(glob.glob(os.path.join(root, 'pitfalls/en/*.md')))
    pit_zh = sorted(glob.glob(os.path.join(root, 'pitfalls/中文版/*.md')))

    print('== language check ==')
    for f in en_files + pit_en:
        verdict, zh, en = classify(open(f, encoding='utf-8').read())
        if verdict == 'zh':
            problems.append(f'EN body is Chinese: {os.path.relpath(f, root)}')
            print(f"  EN->ZH  {os.path.relpath(f, root)} "
                  f"({zh} chinese lines vs {en} english lines)")
    for f in zh_files + pit_zh:
        verdict, zh, en = classify(open(f, encoding='utf-8').read())
        if verdict == 'en':
            problems.append(f'ZH body is English: {os.path.relpath(f, root)}')
            print(f"  ZH->EN  {os.path.relpath(f, root)} "
                  f"({en} english lines vs {zh} chinese lines)")
    print(f"  {len(en_files)} english docs, {len(zh_files)} chinese docs checked")
    print(f"  {len(pit_en)} english pitfalls, {len(pit_zh)} chinese pitfalls checked")
    return len(en_files) + len(pit_en), len(zh_files) + len(pit_zh)


def compare_documents(en_path, zh_path, root, problems, label):
    """Compare one pair and record every difference."""
    en_h, zh_h = headings(en_path), headings(zh_path)
    rel_en = os.path.relpath(en_path, root)
    rel_zh = os.path.relpath(zh_path, root)

    if len(en_h) != len(zh_h):
        problems.append(f'{label} heading count differs: {rel_en} vs {rel_zh}')
        print(f"  HEADINGS  {rel_en}: {len(en_h)} vs {len(zh_h)} in {rel_zh}")
    elif [l for l, _ in en_h] != [l for l, _ in zh_h]:
        problems.append(f'{label} heading levels differ: {rel_en} vs {rel_zh}')
        print(f"  LEVELS    {rel_en}: levels differ from {rel_zh}")

    en_code = code_blocks(en_path)
    zh_code = code_blocks(zh_path)
    if len(en_code) != len(zh_code):
        problems.append(f'{label} code block count differs: {rel_en} vs {rel_zh}')
        print(f"  BLOCKS    {rel_en}: {len(en_code)} vs {len(zh_code)} in {rel_zh}")
    else:
        drifted = [i for i, (a, b) in enumerate(zip(en_code, zh_code))
                   if normalize_code(a, True) != normalize_code(b, True)]
        if drifted:
            problems.append(f'{label} code differs: {rel_en} vs {rel_zh}')
            print(f"  CODE      {rel_en}: block(s) {drifted} differ from {rel_zh}")


def check_code_blocks(root, problems):
    """Every code block must have balanced braces.

    A block that does not balance was cut short. This runs over every file
    rather than over the paired ones: a document that has drifted far enough
    to lose its partner would otherwise escape every check.
    """
    print('\n== code block check ==')
    files = []
    for pattern in ('docs/en/*.md', 'docs/中文版/*.md',
                    'pitfalls/en/*.md', 'pitfalls/中文版/*.md'):
        files += glob.glob(os.path.join(root, pattern))
    checked = 0
    for path in sorted(files):
        for index, block in enumerate(code_blocks(path)):
            checked += 1
            depth = unbalanced(block)
            if any(v != 0 for v in depth.values()):
                problems.append(
                    f'code block {index + 1} is not balanced: '
                    f'{os.path.relpath(path, root)}')
                print(f"  UNBALANCED {os.path.relpath(path, root)} "
                      f"block {index + 1}: {depth}")
    print(f"  {checked} code block(s) checked")
    return checked


def check_pairs(root, problems):
    en_files = sorted(glob.glob(os.path.join(root, 'docs/en/*.md')))
    zh_files = sorted(glob.glob(os.path.join(root, 'docs/中文版/*.md')))

    by_signature = defaultdict(list)
    for f in zh_files:
        sig = code_signature(f)
        if sig is not None:
            by_signature[sig].append(f)

    # Fallback for documents that carry no code block at all (a glossary or a
    # build-system note): their heading skeleton is the only signal left.
    by_headings = defaultdict(list)
    for f in zh_files:
        by_headings[tuple(level for level, _ in headings(f))].append(f)

    print('\n== pair check ==')
    paired = 0
    unverifiable = []
    for f in en_files:
        sig = code_signature(f)
        cands = by_signature.get(sig, []) if sig is not None else []
        if len(cands) != 1:
            key = tuple(level for level, _ in headings(f))
            fallback = by_headings.get(key, [])
            if len(fallback) == 1:
                cands = fallback
        if len(cands) != 1:
            unverifiable.append(os.path.relpath(f, root))
            continue
        paired += 1
        compare_documents(f, cands[0], root, problems, 'docs')

    print(f"  {paired}/{len(en_files)} pairs verified")
    if unverifiable:
        print(f"  {len(unverifiable)} doc(s) could not be matched (not a failure):")
        for name in unverifiable:
            print(f"    {name}")
    return paired


def check_pitfall_pairs(root, problems):
    """Compare the pitfall pairs listed in tools/pitfall_pairs.tsv.

    The pitfall articles are paired by hand rather than by structure: the
    Chinese filenames are translations, so there is no shared spelling to
    match on, and a structural fallback pairs the wrong files when two
    articles happen to use similar examples.
    """
    pairs = load_pair_map(root, os.path.join('tools', 'pitfall_pairs.tsv'))
    print('\n== pair check: pitfalls ==')
    if not pairs:
        print('  no pair map found, skipped')
        return 0

    paired = 0
    for en_name, zh_name in sorted(pairs.items()):
        en_path = os.path.join(root, 'pitfalls/en', en_name)
        zh_path = os.path.join(root, 'pitfalls/中文版', zh_name)
        if not os.path.isfile(en_path) or not os.path.isfile(zh_path):
            problems.append(f'pitfall pair missing a file: {en_name} / {zh_name}')
            print(f"  MISSING  {en_name} / {zh_name}")
            continue
        paired += 1
        compare_documents(en_path, zh_path, root, problems, 'pitfalls')

    # The map has to cover every file on both sides.
    en_names = {os.path.basename(f)
                for f in glob.glob(os.path.join(root, 'pitfalls/en/*.md'))}
    zh_names = {os.path.basename(f)
                for f in glob.glob(os.path.join(root, 'pitfalls/中文版/*.md'))}
    for name in sorted(en_names - set(pairs)):
        problems.append(f'pitfall not in the pair map: {name}')
        print(f"  UNMAPPED  {name}")
    for name in sorted(zh_names - set(pairs.values())):
        problems.append(f'pitfall not in the pair map: {name}')
        print(f"  UNMAPPED  {name}")

    print(f"  {paired}/{len(pairs)} pairs verified")
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
        check_pitfall_pairs(args.repo, problems)
        check_code_blocks(args.repo, problems)

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
