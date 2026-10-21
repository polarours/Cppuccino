#!/usr/bin/env python3
"""Check the relative links of every markdown file in the repository.

A link is broken when the file it points at does not exist, or when it names
an anchor that the target file does not define. External links (http, https,
mailto) and pure in-page anchors are not checked.

The checker mirrors the behaviour of the link-checker project under
cpp-mastery-roadmap/39-stage39/, which is the teaching version of the same
idea: links are extracted without touching the filesystem, and paths are
normalised lexically.

Usage: python3 tools/check_links.py [repo_root]
"""
import argparse
import os
import re
import sys
from collections import defaultdict

LINK_RE = re.compile(r'\[([^\]]*)\]\(([^)\s]+)\)')
FENCE_RE = re.compile(r'^\s{0,3}(`{3,}|~{3,})')
HEADING_RE = re.compile(r'^(#{1,6})\s+(.*)$')
EXTERNAL = ('http://', 'https://', 'mailto:')


def strip_code(text):
    """Remove fenced code blocks and inline code spans.

    A markdown link inside a code block or a code span is an illustration,
    not a link, so it must not be reported.
    """
    out = []
    fence = None
    for line in text.splitlines():
        m = FENCE_RE.match(line)
        if fence is None:
            if m:
                fence = m.group(1)[0]
                out.append('')
                continue
            # blank out inline code spans
            out.append(re.sub(r'`[^`]*`', '', line))
        else:
            if m and m.group(1)[0] == fence:
                fence = None
            out.append('')
    return '\n'.join(out)


def normalize(path):
    """Resolve "." and ".." lexically, without touching the filesystem."""
    absolute = path.startswith('/')
    parts = []
    for segment in path.split('/'):
        if segment in ('', '.'):
            continue
        if segment == '..':
            if parts and parts[-1] != '..':
                parts.pop()
            elif not absolute:
                parts.append('..')
        else:
            parts.append(segment)
    out = '/' + '/'.join(parts) if absolute else '/'.join(parts)
    return out if out else ('/' if absolute else '.')


def anchors_of(path):
    """The set of anchors a markdown file defines, lowercased and slugged."""
    try:
        text = open(path, encoding='utf-8', errors='replace').read()
    except OSError:
        return set()
    out = set()
    for line in text.splitlines():
        m = HEADING_RE.match(line)
        if not m:
            continue
        title = m.group(2).strip()
        # strip trailing hashes of a closed ATX heading
        title = re.sub(r'\s*#+$', '', title).strip()
        slug = re.sub(r'[^\w\s-]', '', title.lower())
        slug = re.sub(r'[\s]+', '-', slug)
        out.add(slug)
    return out


def submodule_paths(root):
    """The directories that are git submodules of this repository."""
    paths = set()
    gitmodules = os.path.join(root, '.gitmodules')
    if os.path.isfile(gitmodules):
        for line in open(gitmodules, encoding='utf-8'):
            m = re.match(r'\s*path\s*=\s*(.+?)\s*$', line)
            if m:
                paths.add(m.group(1))
    return paths


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', nargs='?', default=os.getcwd())
    args = parser.parse_args()
    root = os.path.abspath(args.root)

    skip_dirs = {'.git', 'build', '.workbuddy'}
    md_files = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in skip_dirs]
        for name in filenames:
            if name.endswith('.md'):
                md_files.append(os.path.join(dirpath, name))

    submodules = submodule_paths(root)
    # A submodule that was not checked out holds no files, so a link into it
    # cannot be verified. Those are reported separately instead of as broken.
    unpopulated = {p for p in submodules
                    if not os.path.isfile(os.path.join(root, p, 'CMakeLists.txt'))
                    and not os.path.isfile(os.path.join(root, p, 'README.md'))}

    anchor_cache = {}
    broken = []
    unverified = []
    total = 0

    for path in sorted(md_files):
        text = strip_code(open(path, encoding='utf-8', errors='replace').read())
        for m in LINK_RE.finditer(text):
            target = m.group(2).strip()
            if target.startswith(EXTERNAL):
                continue
            if target.startswith('#'):
                anchor = target[1:]
                if not anchor:
                    continue
                if path not in anchor_cache:
                    anchor_cache[path] = anchors_of(path)
                if anchor.lower() not in anchor_cache[path]:
                    broken.append((os.path.relpath(path, root), target,
                                   'anchor not found'))
                total += 1
                continue

            raw, _, anchor = target.partition('#')
            if not raw:
                continue
            total += 1
            if raw.startswith('/'):
                resolved = os.path.join(root, raw.lstrip('/'))
            else:
                resolved = os.path.normpath(os.path.join(os.path.dirname(path), raw))
            if not os.path.exists(resolved):
                # Is the target inside a submodule that was not checked out?
                inside = [s for s in unpopulated
                          if resolved.startswith(os.path.join(root, s) + os.sep)]
                if inside:
                    unverified.append((os.path.relpath(path, root), target,
                                       inside[0]))
                    continue
                broken.append((os.path.relpath(path, root), target,
                               'file not found'))
                continue
            if anchor and os.path.isfile(resolved) and resolved.endswith('.md'):
                if resolved not in anchor_cache:
                    anchor_cache[resolved] = anchors_of(resolved)
                if anchor.lower() not in anchor_cache[resolved]:
                    broken.append((os.path.relpath(path, root), target,
                                   'anchor not found'))

    print(f'checked {total} relative link(s) in {len(md_files)} markdown file(s)')
    if unverified:
        print(f'\n{len(unverified)} link(s) point into a submodule that was not '
              'checked out, so they could not be verified:')
        for source, target, submodule in unverified:
            print(f'  {source} -> {target}  (inside {submodule})')
    if broken:
        print(f'\n{len(broken)} broken link(s):')
        for source, target, reason in broken:
            print(f'  {source} -> {target}  ({reason})')
        return 1
    print('no broken links')
    return 0


if __name__ == '__main__':
    sys.exit(main())
