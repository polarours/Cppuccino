#!/usr/bin/env python3
"""Run every project's tests under AddressSanitizer and UndefinedBehaviorSanitizer.

The ordinary build runs the tests, but a test that passes can still be
reading freed memory or doing something the standard does not define. The
sanitizers turn those into failures.

ThreadSanitizer is a separate pass: it looks for data races, which is a
different question from the one the other two answer. It is asked for with
--tsan, because a few projects report races that are not theirs to fix.

Usage: python3 tools/run_sanitizers.py [--jobs N] [--stage NAME] [--tsan]
"""
import argparse
import glob
import os
import shutil
import subprocess
import sys
import tempfile
import time
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')
ASAN = '-fsanitize=address,undefined -fno-omit-frame-pointer -g'
TSAN = '-fsanitize=thread -fno-omit-frame-pointer -g -O1'


def projects(stage=None, skip=()):
    found = []
    for dirpath, dirnames, filenames in os.walk(ROADMAP):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' not in filenames:
            continue
        if stage and not os.path.relpath(dirpath, ROADMAP).startswith(stage):
            continue
        found.append(dirpath)
    shell = os.path.join(REPO, 'tinyshell')
    if not stage and os.path.isfile(os.path.join(shell, 'CMakeLists.txt')):
        found.append(shell)
    skip = set(skip)
    return sorted(p for p in found
                  if os.path.relpath(p, REPO) not in skip
                  and os.path.basename(p) not in skip)


def run_one(project, workdir, sanitizers=ASAN, markers=None):
    """Return (ok, detail) for one project under the sanitizers."""
    name = os.path.relpath(project, REPO)
    build = os.path.join(workdir, name.replace('/', '_'))
    log = build + '.log'

    def call(cmd, **kwargs):
        with open(log, 'a') as handle:
            return subprocess.run(cmd, stdout=handle, stderr=subprocess.STDOUT,
                                  **kwargs).returncode

    if call(['cmake', '-S', project, '-B', build, '-DBUILD_TESTING=ON',
             f'-DCMAKE_CXX_FLAGS={sanitizers}']) != 0:
        return False, 'configure failed'
    if call(['cmake', '--build', build, '-j', '2']) != 0:
        return False, 'build failed'

    result = subprocess.run(['ctest', '--test-dir', build, '--output-on-failure'],
                            capture_output=True, text=True, timeout=300)
    if result.returncode != 0:
        detail = ''
        for line in result.stdout.splitlines():
            if 'runtime error' in line or 'AddressSanitizer' in line or \
                    'ThreadSanitizer' in line or 'SUMMARY:' in line:
                detail = line.strip()[:150]
                break
        return False, detail or 'the test failed'

    # A sanitizer report can still be printed while the test passes.
    with open(log) as handle:
        content = handle.read()
    for marker in (markers or ('runtime error:', 'AddressSanitizer:', 'LeakSanitizer:')):
        if marker in content:
            for line in content.splitlines():
                if marker in line:
                    return False, line.strip()[:150]
    return True, ''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', '-j', type=int, default=os.cpu_count() or 2)
    parser.add_argument('--stage')
    parser.add_argument('--tsan', action='store_true',
                        help='look for data races instead of memory errors')
    parser.add_argument('--skip', action='append', default=[],
                        help='a project to leave out, by name or by path')
    args = parser.parse_args()

    sanitizers = TSAN if args.tsan else ASAN
    markers = ('ThreadSanitizer:',) if args.tsan else \
        ('runtime error:', 'AddressSanitizer:', 'LeakSanitizer:')

    todo = projects(args.stage, args.skip)
    workdir = tempfile.mkdtemp(prefix='cppuccino-sanitize-')
    failures = []
    started = time.time()

    try:
        with ThreadPoolExecutor(max_workers=args.jobs) as pool:
            results = list(pool.map(
                lambda p: (p, run_one(p, workdir, sanitizers, markers)), todo))

        for project, (ok, detail) in results:
            name = os.path.relpath(project, REPO)
            if ok:
                print(f'  {name:<58} clean')
            else:
                print(f'  {name:<58} {detail}')
                failures.append((name, detail))
    finally:
        shutil.rmtree(workdir, ignore_errors=True)

    kind = 'the thread sanitizer' if args.tsan else 'the sanitizers'
    print()
    print(f'{len(todo) - len(failures)}/{len(todo)} projects clean under '
          f'{kind}, in {time.time() - started:.0f}s')
    if failures:
        print(f'\n{len(failures)} project(s) with a finding:')
        for name, detail in failures:
            print(f'  {name}: {detail}')
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
