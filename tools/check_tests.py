#!/usr/bin/env python3
"""Check two things about the test files.

A test function that is defined but never called looks like coverage and is
not: it passes, it prints nothing, and it catches nothing. Two of them
slipped into this repository while the tests were being added.

A test that fails has to say so. The expect helper throws a
std::runtime_error, and a main that catches a std::string does not catch
it: the program ends with "terminate called after throwing an instance
of 'std::runtime_error'" and the message the reader needs is never
printed. Ninety of the test files had that.

Usage: python3 tools/check_tests.py [repo_root]
"""
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')

DEFINITION_RE = re.compile(r'^\s*void\s+(test_[A-Za-z0-9_]+)\s*\(', re.M)
CALL_RE = re.compile(r'\b(test_[A-Za-z0-9_]+)\s*\(')
THROW_RE = re.compile(r'throw\s+(std::[A-Za-z_:<>]+)')
CATCH_RE = re.compile(r'catch\s*\(\s*const\s+([A-Za-z_:<>]+)\s*&')

# A type that every other one can be caught as.
BASE_TYPES = ('std::exception',)


def projects(roadmap):
    for dirpath, dirnames, filenames in os.walk(roadmap):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            yield dirpath


def check_calls(project, problems):
    """Every test function has to be called."""
    tests_dir = os.path.join(project, 'tests')
    if not os.path.isdir(tests_dir):
        return
    text = ''
    for name in sorted(os.listdir(tests_dir)):
        if name.endswith(('.cpp', '.hpp')):
            with open(os.path.join(tests_dir, name), encoding='utf-8',
                      errors='replace') as handle:
                text += handle.read()
    if not text:
        return
    for name in sorted(set(DEFINITION_RE.findall(text))):
        if len(CALL_RE.findall(text)) and \
                len(re.findall(r'\b' + re.escape(name) + r'\s*\(', text)) < 2:
            problems.append(f'{os.path.relpath(project, REPO)}: {name} is '
                            f'defined but never called')


def check_failure_is_reported(project, problems):
    """A failing test has to be able to say what went wrong."""
    tests_dir = os.path.join(project, 'tests')
    if not os.path.isdir(tests_dir):
        return
    for name in sorted(os.listdir(tests_dir)):
        if not name.endswith('.cpp'):
            continue
        path = os.path.join(tests_dir, name)
        with open(path, encoding='utf-8', errors='replace') as handle:
            text = handle.read()
        thrown = set(THROW_RE.findall(text))
        caught = set(CATCH_RE.findall(text))
        if not thrown or not caught:
            continue
        uncaught = {t for t in thrown
                    if t not in caught and
                    not any(c in BASE_TYPES for c in caught)}
        if uncaught:
            problems.append(
                f'{os.path.relpath(path, REPO)}: throws '
                f'{", ".join(sorted(uncaught))}, which the handler for '
                f'{", ".join(sorted(caught))} does not catch')


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else REPO
    roadmap = os.path.join(root, 'cpp-mastery-roadmap')
    problems = []

    print('== tests ==')
    count = 0
    for project in sorted(projects(roadmap)):
        count += 1
        check_calls(project, problems)
        check_failure_is_reported(project, problems)
    print(f'  {count} project(s) checked')

    if problems:
        print(f'\n{len(problems)} problem(s):')
        for problem in problems:
            print(f'  {problem}')
        return 1
    print('\nevery test function is called, and every failure can be reported')
    return 0


if __name__ == '__main__':
    sys.exit(main())
