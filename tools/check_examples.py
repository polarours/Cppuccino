#!/usr/bin/env python3
"""Compile every example under examples/.

Each example states how to build itself in its header comment, for example
"// Compile with: g++ -std=c++20 -o demo demo.cpp". This script reads that
declaration and uses it, so a header that lies about its own requirements is
reported as well. Examples without a declaration are compiled with the first
standard that works.

The examples are compiled and linked but not run: several of them expect
input or start a server.

Usage: python3 tools/check_examples.py [--jobs N]
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXAMPLES = os.path.join(REPO, 'examples')

# Examples that build on a roadmap project and need its headers and sources.
PROJECT_DEPENDENT = {
    'task-scheduler-error-handling.cpp': (
        'cpp-mastery-roadmap/05-stage5/task-scheduler',
        ['src/task_scheduler.cpp'],
    ),
    'web-server-benchmark.cpp': (
        'cpp-mastery-roadmap/04-stage4/web-server',
        ['src/web_server.cpp'],
    ),
    'connection-pool-concurrency.cpp': (
        'cpp-mastery-roadmap/01-beginner/student-management/v1',
        [],
    ),
}

STANDARD_RE = re.compile(r'-std=c\+\+(\d+)')
DECL_RE = re.compile(r'^\s*//.*\bcompile\b', re.IGNORECASE)

# Standards tried, in order, for an example that does not declare one.
FALLBACK_STANDARDS = ['20', '23', '26']


def declared_standard(path):
    """The -std= flag the header declares, or None when it declares none."""
    with open(path, encoding='utf-8', errors='replace') as handle:
        for _ in range(40):          # the declaration lives in the header
            line = handle.readline()
            if not line:
                break
            if DECL_RE.match(line):
                match = STANDARD_RE.search(line)
                if match:
                    return match.group(1)
    return None


def compile_commands(path, standard, workdir):
    """The g++ command line for one example."""
    name = os.path.basename(path)
    binary = os.path.join(workdir, name[:-4])
    cmd = ['g++', f'-std=c++{standard}', '-o', binary, path]

    project = PROJECT_DEPENDENT.get(name)
    if project:
        directory, sources = project
        cmd.insert(2, f'-I{os.path.join(REPO, directory)}/include')
        for source in sources:
            cmd.append(os.path.join(REPO, directory, source))
    return cmd


def check_one(path, workdir):
    """Return (ok, detail)."""
    name = os.path.basename(path)
    standard = declared_standard(path)

    if standard:
        cmd = compile_commands(path, standard, workdir)
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode == 0:
            return True, f'ok (-std=c++{standard})'
        first = next((l for l in result.stderr.splitlines() if ' error:' in l),
                     result.stderr.splitlines()[0] if result.stderr else '?')
        return False, f'failed with the declared -std=c++{standard}: {first.strip()[:110]}'

    errors = []
    for candidate in FALLBACK_STANDARDS:
        cmd = compile_commands(path, candidate, workdir)
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode == 0:
            return True, f'ok (-std=c++{candidate})'
        first = next((l for l in result.stderr.splitlines() if ' error:' in l),
                     result.stderr.splitlines()[0] if result.stderr else '?')
        errors.append(f'{candidate}: {first.strip()[:80]}')
    return False, 'no standard works -- ' + ' | '.join(errors)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', '-j', type=int, default=os.cpu_count() or 2)
    args = parser.parse_args()

    sources = sorted(os.path.join(EXAMPLES, name)
                     for name in os.listdir(EXAMPLES) if name.endswith('.cpp'))
    if not sources:
        print('no examples found')
        return 1

    workdir = tempfile.mkdtemp(prefix='cppuccino-examples-')
    failures = []

    try:
        with ThreadPoolExecutor(max_workers=args.jobs) as pool:
            results = list(pool.map(
                lambda p: (p, check_one(p, workdir)), sources))

        for path, (ok, detail) in results:
            name = os.path.basename(path)
            if ok:
                print(f'  {name:<48} {detail}')
            else:
                print(f'  {name:<48} FAIL {detail}')
                failures.append((name, detail))
    finally:
        shutil.rmtree(workdir, ignore_errors=True)

    print()
    print(f'{len(sources) - len(failures)}/{len(sources)} examples compiled')
    if failures:
        print(f'\n{len(failures)} failure(s):')
        for name, detail in failures:
            print(f'  {name}: {detail}')
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
