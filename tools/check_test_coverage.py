#!/usr/bin/env python3
"""Check that every test function is actually run.

A test function that is defined but never called looks like coverage and is
not: it passes, it prints nothing, and it catches nothing. Two of them
slipped into this repository while the tests were being added.

The check is deliberately narrow: a test function has to be called from
the main of the same project. A test that is called from another test is
fine, and so is one that a framework calls.

Usage: python3 tools/check_test_coverage.py [repo_root]
"""
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')

DEFINITION_RE = re.compile(r'^\s*void\s+(test_[A-Za-z0-9_]+)\s*\(', re.M)
CALL_RE = re.compile(r'\b(test_[A-Za-z0-9_]+)\s*\(')


def projects(roadmap):
    for dirpath, dirnames, filenames in os.walk(roadmap):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            yield dirpath


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else REPO
    roadmap = os.path.join(root, 'cpp-mastery-roadmap')
    problems = []
    checked = 0
    total = 0

    print('== test functions ==')
    for project in sorted(projects(roadmap)):
        tests_dir = os.path.join(project, 'tests')
        if not os.path.isdir(tests_dir):
            continue
        sources = [os.path.join(tests_dir, f) for f in sorted(os.listdir(tests_dir))
                   if f.endswith(('.cpp', '.hpp'))]
        if not sources:
            continue
        checked += 1
        text = '\n'.join(open(path, encoding='utf-8', errors='replace').read()
                         for path in sources)
        defined = set(DEFINITION_RE.findall(text))
        if not defined:
            continue
        # A call is a name followed by an opening parenthesis, and not the
        # definition itself.
        calls = set(CALL_RE.findall(text))
        for name in sorted(defined):
            total += 1
            # The definition counts as one occurrence; a call is another.
            if text.count(name + '(') < 2:
                problems.append(f'{os.path.relpath(project, root)}: '
                                f'{name} is defined but never called')

    print(f'  {checked} project(s) with tests checked')
    print(f'  {total} test function(s) found')
    if problems:
        print(f'\n{len(problems)} problem(s):')
        for problem in problems:
            print(f'  {problem}')
        return 1
    print('\nevery test function is called')
    return 0


if __name__ == '__main__':
    sys.exit(main())
