#!/usr/bin/env python3
"""Give every stage of the roadmap index a reading list.

The index listed the projects of every stage, but only stages one to four
had reading lists, so most of the articles could not be found from it. This
reads tools/overview_reading_map.tsv and, for every stage, makes sure the
articles and pitfalls of that stage are listed.

A stage that already has a reading list gets the missing entries added to
it; a stage that has none gets a fresh list inserted before its project
list. Both language halves are updated, so the two indexes stay in step.

Usage: python3 tools/write_overview_reading_lists.py [--dry-run]
"""
import argparse
import glob
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OVERVIEW_EN = os.path.join(REPO, 'cpp-mastery-roadmap', '00-overview.md')
OVERVIEW_ZH = os.path.join(REPO, 'cpp-mastery-roadmap', '00-overview.zh-CN.md')
MAP = os.path.join(REPO, 'tools', 'overview_reading_map.tsv')

# The stage directories that are not named after their number.
ODD_STAGE_NAMES = {'01-beginner', '02-intermediate', '03-advanced'}


def load_map():
    """stage -> (docs, pitfalls), as English file names."""
    stages = {}
    for line in open(MAP, encoding='utf-8'):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        stage, kind, slug = line.split('\t')
        docs, pitfalls = stages.setdefault(stage, ([], []))
        (docs if kind == 'doc' else pitfalls).append(slug)
    return stages


# The few documents that share no name and carry no English gloss in their
# Chinese title.
DOC_NAME_OVERRIDES = {
    'glossary.md': '术语表.md',
}


def english_to_chinese_docs():
    """English document file name -> Chinese document file name."""
    import importlib.util
    spec = importlib.util.spec_from_file_location(
        'check_docs', os.path.join(REPO, 'tools', 'check_docs.py'))
    check_docs = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(check_docs)

    by_signature = {}
    for path in glob.glob(os.path.join(REPO, 'docs', '中文版', '*.md')):
        sig = check_docs.code_signature(path)
        if sig is not None:
            by_signature.setdefault(sig, []).append(path)

    out = dict(DOC_NAME_OVERRIDES)
    for path in glob.glob(os.path.join(REPO, 'docs', 'en', '*.md')):
        if os.path.basename(path) in out:
            continue
        sig = check_docs.code_signature(path)
        cands = by_signature.get(sig, []) if sig is not None else []
        if len(cands) == 1:
            out[os.path.basename(path)] = os.path.basename(cands[0])
            continue
        # A document with no code block at all cannot be matched by code.
        # The Chinese title carries the English name in parentheses, which
        # identifies it exactly.
        english = re.sub(r'[^a-z0-9]+', '',
                         check_docs.title(path).lower())
        for other in glob.glob(os.path.join(REPO, 'docs', '中文版', '*.md')):
            gloss = check_docs.english_gloss(check_docs.headings(other))
            if gloss and gloss == english:
                out[os.path.basename(path)] = os.path.basename(other)
                break
    return out


def english_to_chinese_pitfalls():
    """English pitfall file name -> Chinese pitfall file name."""
    out = {}
    for line in open(os.path.join(REPO, 'tools', 'pitfall_pairs.tsv'),
                     encoding='utf-8'):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        parts = line.split('\t')
        if len(parts) == 2:
            out[parts[0].strip()] = parts[1].strip()
    return out


def article_title(path):
    text = open(path, encoding='utf-8', errors='replace').read()
    m = re.search(r'^#\s+(.+)$', text, re.M)
    return m.group(1).strip() if m else os.path.basename(path)[:-3]


def link_line(slug, kind, zh):
    """The markdown list entry for one document or pitfall."""
    if kind == 'doc':
        names = english_to_chinese_docs()
        folder = '中文版' if zh else 'en'
        name = names.get(slug + '.md') if zh else slug + '.md'
        if not name:
            return None
        label = article_title(os.path.join(REPO, 'docs', folder, name))
        return f'- [{label}](../docs/{folder}/{name})'
    names = english_to_chinese_pitfalls()
    folder = '中文版' if zh else 'en'
    name = names.get(slug + '.md') if zh else slug + '.md'
    if not name:
        return None
    label = article_title(os.path.join(REPO, 'pitfalls', folder, name))
    return f'- [{label}](../pitfalls/{folder}/{name})'


def section_bounds(text, stage, zh):
    """The start and end of a stage's section, in the given index."""
    number = int(stage[:2])
    if zh:
        # The Chinese index spells the first stages with Chinese numerals.
        numeral = {1: '一', 2: '二', 3: '三', 4: '四'}.get(number, str(number))
        pattern = rf'^## 第{numeral}阶段[^\n]*\n'
    else:
        pattern = rf'^## Stage {number}:[^\n]*\n'
    m = re.search(pattern, text, re.M)
    if not m:
        return None
    start = m.start()
    nxt = re.search(r'^## ', text[m.end():], re.M)
    end = m.end() + nxt.start() if nxt else len(text)
    return start, end


def merge_into(text, stage, docs, pitfalls, zh):
    """Add the missing entries to a stage that already has a reading list."""
    bounds = section_bounds(text, stage, zh)
    if not bounds:
        return text, 0
    start, end = bounds
    body = text[start:end]

    added = 0
    for kind, slugs in (('doc', docs), ('pitfall', pitfalls)):
        if not slugs:
            continue
        label = ('Pitfalls:' if kind == 'pitfall' else 'Read:') if not zh else \
                ('对应 pitfalls：' if kind == 'pitfall' else '建议阅读：')
        m = re.search(rf'^{re.escape(label)}\n\n((?:- .*\n)*)', body, re.M)
        if not m:
            continue
        existing = m.group(1)
        # Compare by link target: the same document may already be listed
        # under a slightly different label.
        listed = set(re.findall(r'\]\(([^)]+)\)', existing))
        lines = []
        for slug in slugs:
            entry = link_line(slug, kind, zh)
            if not entry:
                continue
            target = re.search(r'\]\(([^)]+)\)', entry).group(1)
            if target in listed:
                continue
            listed.add(target)
            lines.append(entry + '\n')
        if not lines:
            continue
        addition = ''.join(lines)
        body = body[:m.end(1)] + addition + body[m.end(1):]
        added += len(lines)

    if added:
        text = text[:start] + body + text[end:]
    return text, added


def insert_new(text, stage, docs, pitfalls, zh):
    """Give a stage that has no reading list one."""
    bounds = section_bounds(text, stage, zh)
    if bounds:
        start, end = bounds
        body = text[start:end]
    else:
        # From stage five on there is no narrative section: the project list
        # sits under the "Current Status" heading. The reading list goes
        # immediately before that project list.
        anchor = re.search(rf'^###[^\n]*[（(]\s*{re.escape(stage)}\s*[）)]',
                           text, re.M)
        if not anchor:
            return text, 0
        start = anchor.start()
        end = anchor.end()
        body = text[start:end]

    # The list goes just before the stage's project list, or at the end of
    # the section when there is none.
    m = re.search(r'^###[^\n]*\n', body, re.M)
    if bounds:
        insert_at = m.start() if m else len(body)
    else:
        insert_at = 0

    block = []
    for kind, slugs in (('doc', docs), ('pitfall', pitfalls)):
        entries = [e for e in (link_line(s, kind, zh) for s in slugs) if e]
        if not entries:
            continue
        if zh:
            block.append('建议阅读：' if kind == 'doc' else '对应 pitfalls：')
        else:
            block.append('Read:' if kind == 'doc' else 'Pitfalls:')
        block.append('')
        block.extend(entries)
        block.append('')
    if not block:
        return text, 0

    # Keep a blank line before the block.
    if insert_at and body[insert_at - 1] != '\n':
        block.insert(0, '')
    body = body[:insert_at] + '\n'.join(block) + '\n' + body[insert_at:]
    return text[:start] + body + text[end:], 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()

    stages = load_map()
    changed = 0

    for path, zh in ((OVERVIEW_EN, False), (OVERVIEW_ZH, True)):
        text = open(path, encoding='utf-8').read()
        for stage, (docs, pitfalls) in sorted(stages.items()):
            if not docs and not pitfalls:
                continue
            text, added = merge_into(text, stage, docs, pitfalls, zh)
            if added:
                changed += 1
            else:
                text, added = insert_new(text, stage, docs, pitfalls, zh)
                if added:
                    changed += 1
        if not args.dry_run:
            open(path, 'w', encoding='utf-8').write(text)

    print(f'{changed} stage(s) {"would be " if args.dry_run else ""}updated')
    return 0


if __name__ == '__main__':
    sys.exit(main())
