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


# The examples that build on a project need its headers and its sources,
# so they cannot be checked on their own. The example checker compiles them
# with the project, and this one leaves them alone.
PROJECT_EXAMPLES = {
    'task-scheduler-error-handling.cpp',
    'web-server-benchmark.cpp',
    'connection-pool-concurrency.cpp',
}


def sources():
    """Every .cpp file that belongs to a project, and every example."""
    found = []
    for dirpath, dirnames, filenames in os.walk(ROADMAP):
        dirnames[:] = [d for d in dirnames if d != 'build']
        for name in sorted(filenames):
            if name.endswith('.cpp'):
                found.append(os.path.join(dirpath, name))
    for name in sorted(os.listdir(EXAMPLES)):
        if name.endswith('.cpp') and name not in PROJECT_EXAMPLES:
            found.append(os.path.join(EXAMPLES, name))
    return found


def project_root(path):
    """The directory of the project a file belongs to."""
    current = os.path.dirname(os.path.abspath(path))
    while current != os.path.dirname(current):
        if os.path.isfile(os.path.join(current, 'CMakeLists.txt')):
            return current
        current = os.path.dirname(current)
    return os.path.dirname(os.path.abspath(path))


def include_dirs(path):
    """The headers a file may include.

    The project root is the directory that carries the CMakeLists.txt, and
    every directory below it that holds a header is on the include path.
    The headers of a project usually live in include/, which is a sibling
    of src/ and not a child of it, so looking only below the directory of
    the file misses them.
    """
    # The examples of the repository itself stand alone: they have no
    # project of their own. The examples subdirectory of a project is
    # another matter -- that one belongs to its project.
    if os.path.dirname(os.path.abspath(path)) == os.path.abspath(EXAMPLES):
        return []
    root = project_root(path)
    dirs = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d != 'build']
        for name in filenames:
            if name.endswith(('.hpp', '.h')):
                dirs.append(dirpath)
                # The parent as well: a header in include/mini_http/ is
                # included as "mini_http/app.hpp" from the include path
                # include/, not from include/mini_http/.
                dirs.append(os.path.dirname(dirpath))
                break
    dirs += extra_include_dirs(root)
    return sorted(set(dirs))


def extra_include_dirs(root):
    """The include paths the CMakeLists.txt names.

    A project may borrow the headers of another one, as the producer and
    consumer demo does with the ring buffer. Those paths are written as
    ${CMAKE_CURRENT_SOURCE_DIR}/../something/include, and they are not
    below the project.
    """
    cmake = os.path.join(root, 'CMakeLists.txt')
    if not os.path.isfile(cmake):
        return []
    with open(cmake, encoding='utf-8', errors='replace') as handle:
        text = handle.read()
    out = []
    for match in re.finditer(r'\$\{CMAKE_CURRENT_SOURCE_DIR\}(/[^\s"\')]+)*', text):
        relative = match.group(0).replace('${CMAKE_CURRENT_SOURCE_DIR}', root)
        resolved = os.path.normpath(relative)
        if os.path.isdir(resolved):
            out.append(resolved)
            out.append(os.path.dirname(resolved))
    return out


def standard(path):
    """The standard the project asks for.

    The CMakeLists.txt knows it; the header of the file usually does not
    mention it at all, and guessing C++17 for a file that needs C++20 says
    nothing about the file.
    """
    root = project_root(path)
    cmake = os.path.join(root, 'CMakeLists.txt')
    if os.path.isfile(cmake):
        with open(cmake, encoding='utf-8', errors='replace') as handle:
            match = re.search(r'set\s*\(\s*CMAKE_CXX_STANDARD\s+(\d+)', handle.read())
        if match:
            return match.group(1)
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
    if result.returncode != 0 and not found:
        # A file that does not compile is a problem of its own. Reporting
        # only the warnings would let it pass as a quiet one.
        first = next((l for l in result.stderr.splitlines() if ' error:' in l),
                     result.stderr.splitlines()[0] if result.stderr else '?')
        found.append(('does not compile', first.strip()[:150]))
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', '-j', type=int, default=os.cpu_count() or 2)
    args = parser.parse_args()

    allowed = load_allowlist()
    paths = sources()
    problems = []
    used = set()

    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results = list(pool.map(check, paths))

    for path, warnings in zip(paths, results):
        rel = os.path.relpath(path, REPO)
        for kind, line in warnings:
            if kind in allowed.get(rel, ()):
                used.add((rel, kind))
                continue
            problems.append((rel, line))

    # An entry that no longer matches anything is a leftover: the warning it
    # allowed has been fixed, and the entry would hide it coming back.
    stale = [(rel, kind) for rel, kinds in allowed.items() for kind in kinds
             if (rel, kind) not in used]

    print('== warnings ==')
    print(f'  {len(paths)} file(s) compiled with the warnings on')
    print(f'  {len(allowed)} file(s) with an expected warning')
    if stale:
        print(f'\n{len(stale)} expected warning(s) that are not there any more:')
        for rel, kind in sorted(stale):
            print(f'  {rel}  {kind}')
    if problems:
        print(f'\n{len(problems)} unexpected warning(s):')
        for rel, line in problems:
            print(f'  {rel}\n    {line}')
    if problems or stale:
        return 1
    print('\nevery project source and every example is quiet')
    return 0


if __name__ == '__main__':
    sys.exit(main())
