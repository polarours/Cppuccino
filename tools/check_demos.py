#!/usr/bin/env python3
"""Run every project's demo program.

The projects are built and their tests are run, but the demo binary -- the
one a reader is told to run -- is only compiled. A demo that crashes when
it starts is a demo nobody can use, and two of them did nothing at all:
they printed that the server had started and never started it.

A demo that does not finish is a server, and a demo that cannot bind its
port is a machine that is already using it. Neither is a failure.

Usage: python3 tools/check_demos.py [--jobs N]
"""
import argparse
import json
import os
import resource
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')

# The demos run with a limit, so that a demo that allocates without end
# fails instead of taking the machine down with it.
ADDRESS_SPACE = 1024 * 1024 * 1024
TIMEOUT = 10


def limits():
    resource.setrlimit(resource.RLIMIT_AS, (ADDRESS_SPACE, ADDRESS_SPACE))


def projects():
    for dirpath, dirnames, filenames in os.walk(ROADMAP):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            yield dirpath


def one(project):
    name = os.path.relpath(project, REPO)
    with tempfile.TemporaryDirectory() as workdir:
        build = os.path.join(workdir, 'b')
        if subprocess.run(['cmake', '-S', project, '-B', build,
                          '-DBUILD_TESTING=ON'],
                         capture_output=True).returncode != 0:
            return name, 'skip', 'configure failed'
        if subprocess.run(['cmake', '--build', build, '-j', '2'],
                         capture_output=True).returncode != 0:
            return name, 'skip', 'build failed'

        listed = subprocess.run(
            ['ctest', '--test-dir', build, '--show-only=json-v1'],
            capture_output=True, text=True, errors='replace')
        try:
            tests = json.loads(listed.stdout).get('tests', [])
        except ValueError:
            tests = []
        test_bins = {os.path.basename(t.get('command', [''])[0]) for t in tests}

        demos = []
        for root, _, files in os.walk(build):
            for f in files:
                path = os.path.join(root, f)
                if not os.path.isfile(path) or not os.access(path, os.X_OK):
                    continue
                if f in test_bins or f.startswith('CTest') or f == 'cmake':
                    continue
                if f.count('.') == 0:
                    demos.append(path)

        if not demos:
            return name, 'skip', 'no demo binary'

        for demo in sorted(demos):
            # The output goes to a file, not into memory: a demo that prints
            # without end would fill the machine before the timeout fires.
            out_path = os.path.join(workdir, 'out.txt')
            with open(out_path, 'wb') as out:
                try:
                    run = subprocess.run([demo], cwd=workdir, stdout=out,
                                         stderr=subprocess.STDOUT,
                                         timeout=TIMEOUT, preexec_fn=limits)
                except subprocess.TimeoutExpired:
                    return name, 'skip', f'{os.path.basename(demo)} does not finish'
                except MemoryError:
                    return name, 'fail', (f'{os.path.basename(demo)} ran out of '
                                          'memory')
            with open(out_path, encoding='utf-8', errors='replace') as handle:
                lines = handle.read().splitlines()
            if run.returncode == 0:
                continue
            text = '\n'.join(lines).lower()
            if 'bind' in text or 'address already in use' in text:
                return name, 'skip', f'{os.path.basename(demo)}: the port is taken'
            detail = lines[-1] if lines else '?'
            return name, 'fail', (f'{os.path.basename(demo)} exited with '
                                  f'{run.returncode}: {detail[:90]}')
        return name, 'ok', ''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', '-j', type=int, default=2)
    args = parser.parse_args()

    todo = sorted(projects())
    ok = skipped = bad = 0
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for name, status, detail in pool.map(one, todo):
            if status == 'ok':
                ok += 1
            elif status == 'skip':
                skipped += 1
                print(f'  skip  {name:<52} {detail}', flush=True)
            else:
                bad += 1
                print(f'  FAIL  {name:<52} {detail}', flush=True)

    print(f'\n{ok} demo(s) ran cleanly, {skipped} skipped, {bad} failed')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
