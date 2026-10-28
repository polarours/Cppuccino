#!/usr/bin/env python3
"""Configure, build and test every project in the roadmap.

Each project under cpp-mastery-roadmap/ has its own CMakeLists.txt and is
self-contained, so they are handled one at a time. A project that fails to
configure, fails to build or fails its tests is reported and the run
continues, so a single broken project does not hide the rest.

Usage:
    python3 tools/build_all_projects.py [--jobs N] [--stage NAME] [--keep]
"""
import argparse
import os
import shutil
import subprocess
import sys
import tempfile
import time

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')


def find_projects(stage=None):
    """Every directory under the roadmap that carries a CMakeLists.txt.

    The tinyshell submodule is included as well: it is a project of this
    repository even though it lives outside the roadmap.
    """
    projects = []
    for dirpath, dirnames, filenames in os.walk(ROADMAP):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' not in filenames:
            continue
        rel = os.path.relpath(dirpath, ROADMAP)
        if stage and not rel.startswith(stage):
            continue
        projects.append(dirpath)

    if not stage:
        shell = os.path.join(REPO, 'tinyshell')
        if os.path.isfile(os.path.join(shell, 'CMakeLists.txt')):
            projects.append(shell)
    return sorted(projects)


def run(cmd, log_path, timeout=None):
    with open(log_path, 'w') as log:
        try:
            return subprocess.run(cmd, stdout=log, stderr=subprocess.STDOUT,
                                  timeout=timeout).returncode
        except subprocess.TimeoutExpired:
            log.write(f'\n*** {cmd[0]} did not finish in {timeout} seconds\n')
            log.flush()
            return 124


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', '-j', type=int, default=os.cpu_count() or 2)
    parser.add_argument('--stage', help='only handle this stage directory')
    parser.add_argument('--keep', action='store_true',
                        help='keep the build directories for inspection')
    args = parser.parse_args()

    projects = find_projects(args.stage)
    if not projects:
        print('no projects found')
        return 1

    build_root = tempfile.mkdtemp(prefix='cppuccino-build-')
    failures = []
    started = time.time()

    for index, project in enumerate(projects, 1):
        name = os.path.relpath(project, REPO)
        build_dir = os.path.join(build_root, name.replace('/', '_'))
        log_path = os.path.join(build_root, name.replace('/', '_') + '.log')
        print(f'[{index}/{len(projects)}] {name}', flush=True)

        if run(['cmake', '-S', project, '-B', build_dir, '-DBUILD_TESTING=ON'],
               log_path) != 0:
            failures.append((name, 'configure', log_path))
            print('    configure FAILED')
            continue
        if run(['cmake', '--build', build_dir, '-j', str(args.jobs)],
               log_path) != 0:
            failures.append((name, 'build', log_path))
            print('    build FAILED')
            continue
        # A test that hangs has to fail the run rather than fill the job's
        # whole timeout.
        if run(['ctest', '--test-dir', build_dir, '--output-on-failure'],
               log_path, timeout=300) != 0:
            failures.append((name, 'test', log_path))
            print('    test FAILED')
            continue
        print('    OK', flush=True)

    elapsed = time.time() - started
    print()
    print(f'{len(projects) - len(failures)}/{len(projects)} projects passed '
          f'in {elapsed:.0f}s')
    if failures:
        print(f'\n{len(failures)} failure(s):')
        for name, stage, log_path in failures:
            print(f'  {stage}: {name}  (log: {log_path})')
        if not args.keep:
            print('\nre-run with --keep to keep the build directories')
            return 1

    if not args.keep:
        shutil.rmtree(build_root, ignore_errors=True)
    else:
        print(f'\nbuild directories kept in {build_root}')
    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
