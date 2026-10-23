#!/usr/bin/env python3
"""Compile every project source and every example with the warnings on.

A warning is a mistake that the compiler already found. Three of the
examples break the rules on purpose -- they are the ones that show what a
dangling reference or a slice looks like -- and they are listed in
tools/known_warnings.tsv.

The projects and the other examples are expected to be quiet.

Usage: python3 tools/check_warnings.py [--jobs N]
"""
import argparse
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')
EXAMPLES = os.path.join(REPO, 'examples')
ALLOWLIST = os.path.join(REPO, 'tools', 'known_warnings.tsv')

WARNINGS = ['-Wall', '-Wextra', '-Wpedantic', '-Wshadow', '-Wnon-virtual-dtor',
            '-Wnull-dereference', '-Wold-style-cast', '-Wunused-variable',
            '-Wunused-but-set-variable', '-Wreturn-local-addr',
            '-Wsign-compare']

# A warning that names a header of its own is not a -W kind, so the kind is
# taken from the text in brackets when there is one. The compiler sometimes
# writes a trailing equals sign after the name.
KIND_RE = re.compile(r'\[(-W[a-z0-9=-]+)\]\s*$')


def load_allowlist():
    """The warnings that are expected, as {file: {kind}}."""
    allowed = {}
    if not os.path.isfile(ALLOWLIST):
        return allowed
    with open(ALLOWLIST, encoding='utf-8') as handle:
        for line in handle:
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            parts = line.split('\t')
            if len(parts) != 2:
                print(f'  MALFORMED  {line}')
                continue
            allowed.setdefault(parts[0].strip(), set()).add(parts[1].strip())
    return allowed


def sources():
    """Every .cpp file that belongs to a project, and every example."""
    found = []
    for dirpath, dirnames, filenames in os.walk(ROADMAP):
        dirnames[:] = [d for d in dirnames if d != 'build']
        for name in sorted(filenames):
            if name.endswith('.cpp'):
                found.append(os.path.join(dirpath, name))
    for name in sorted(os.listdir(EXAMPLES)):
        if name.endswith('.cpp'):
            found.append(os.path.join(EXAMPLES, name))
    return found


def include_dirs(path):
    """The headers a file may include, taken from the project it is in."""
    dirs = []
    project = os.path.dirname(path)
    if os.path.basename(os.path.dirname(path)) == 'examples':
        return dirs
    for sub in ('include', 'src', 'tests'):
        base = os.path.join(project, sub)
        if not os.path.isdir(base):
            continue
        for root, _, files in os.walk(base):
            for name in files:
                if name.endswith(('.hpp', '.h')):
                    dirs.append(root)
    return sorted(set(dirs))


def standard(path):
    """The standard the file's own header asks for."""
    try:
        with open(path, encoding='utf-8', errors='replace') as handle:
            head = handle.read(4000)
    except OSError:
        return '17'
    match = re.search(r'-std=c\+\+(\d+)', head)
    return match.group(1) if match else '17'


def check(path):
    """Return the warnings of one file, as (kind, line) pairs."""
    command = ['g++', f'-std=c++{standard(path)}', '-fsyntax-only']
    command += WARNINGS
    command += [f'-I{d}' for d in include_dirs(path)]
    command.append(path)
    result = subprocess.run(command, capture_output=True, text=True,
                            errors='replace')
    found = []
    for line in result.stderr.splitlines():
        if ' warning: ' not in line:
            continue
        text = line.split(' warning: ', 1)[1]
        kind = KIND_RE.search(text)
        if kind:
            # The compiler sometimes writes a trailing equals sign after the
            # name of the warning.
            name = kind.group(1).rstrip('=')
        else:
            name = text.split(' [-W', 1)[0][:40] if ' [-W' in text else text[:40]
        found.append((name, line.strip()[:150]))
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', '-j', type=int, default=os.cpu_count() or 2)
    args = parser.parse_args()

    allowed = load_allowlist()
    paths = sources()
    problems = []

    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results = list(pool.map(check, paths))

    for path, warnings in zip(paths, results):
        rel = os.path.relpath(path, REPO)
        for kind, line in warnings:
            if kind in allowed.get(rel, ()):
                continue
            problems.append((rel, line))

    print('== warnings ==')
    print(f'  {len(paths)} file(s) compiled with the warnings on')
    print(f'  {len(allowed)} file(s) with an expected warning')
    if problems:
        print(f'\n{len(problems)} unexpected warning(s):')
        for rel, line in problems:
            print(f'  {rel}\n    {line}')
        return 1
    print('\nevery project source and every example is quiet')
    return 0


if __name__ == '__main__':
    sys.exit(main())
