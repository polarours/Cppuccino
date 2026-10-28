#!/usr/bin/env python3
"""Check that no two projects use the same port in their tests.

The CI builds the projects in ten shards and runs them at the same time. Two
projects that bind the same port in their tests fight over it, and the one
that loses fails for no reason but the schedule. It looks like a flake, and
it is not.

Usage: python3 tools/check_test_ports.py
"""
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')

# A server that is given a port number: Server(18001), Server("0.0.0.0", 18002)
PORT_RE = re.compile(
    r'\b[A-Za-z_][A-Za-z0-9_]*\s*\(\s*(?:"[^"]*"\s*,\s*)?(\d{4,5})\s*[,)]')

# A file that opens a socket. The numbers in a file that does not are
# buffer sizes and the like, not ports.
SOCKET_RE = re.compile(r'\b(bind|listen|accept|htons|sockaddr_in|socket)\s*\(')

# The ports that are not a port at all.
NOT_A_PORT = {8080, 3128, 443, 80}


def projects():
    for dirpath, dirnames, filenames in os.walk(ROADMAP):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            yield dirpath


def main():
    owners = {}
    problems = []

    for project in sorted(projects()):
        tests = os.path.join(project, 'tests')
        if not os.path.isdir(tests):
            continue
        for name in sorted(os.listdir(tests)):
            if not name.endswith(('.cpp', '.hpp')):
                continue
            path = os.path.join(tests, name)
            with open(path, encoding='utf-8', errors='replace') as handle:
                text = handle.read()
            if not SOCKET_RE.search(text):
                # The numbers in this file are not ports.
                continue
            for number, line in enumerate(text.splitlines(), 1):
                for match in PORT_RE.finditer(line):
                    port = int(match.group(1))
                    if port < 1024 or port in NOT_A_PORT:
                        continue
                    where = (os.path.relpath(path, REPO), number)
                    owners.setdefault(port, []).append(where)

    for port, places in sorted(owners.items()):
        projects_of = {os.path.dirname(p) for p, _ in places}
        if len(projects_of) > 1:
            problems.append((port, places))

    print('== test ports ==')
    print(f'  {len(owners)} port(s) used by the tests')
    if problems:
        print(f'\n{len(problems)} port(s) used by more than one project:')
        for port, places in problems:
            print(f'  port {port}:')
            for path, line in places:
                print(f'    {path}:{line}')
        return 1
    print('\nno two projects use the same port')
    return 0


if __name__ == '__main__':
    sys.exit(main())
