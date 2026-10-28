#!/usr/bin/env python3
"""Compile the C++ example in every project README.

The documents in docs/ and the files in examples/ are checked, but the
example at the bottom of a project README was not: a snippet that names a
type the project does not have, or a variable that is never declared, was
published as if it worked. Four of them did not.

The example is a fragment, so it is tried in two shapes: at file scope, and
wrapped in a main(). A snippet that declares a class or a function only
compiles in the first shape, and a snippet that runs something only
compiles in the second.

Usage: python3 tools/check_readme_examples.py [repo_root]
"""
import argparse
import os
import re
import subprocess
import sys
import tempfile

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')


def projects(roadmap=ROADMAP):
    for dirpath, dirnames, filenames in os.walk(roadmap):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            yield dirpath


def code_blocks(text):
    return re.findall(r'```(\w*)\n(.*?)```', text, re.S)


def include_dirs(project):
    dirs = [os.path.join(project, 'include')]
    for sub in ('include', 'src', 'tests'):
        for root, _, files in os.walk(os.path.join(project, sub)):
            for name in files:
                if name.endswith(('.hpp', '.h')):
                    dirs.append(root)
                    # The parent as well: a header in include/mini_http/ is
                    # included as "mini_http/app.hpp" from the include path
                    # include/, not from include/mini_http/.
                    dirs.append(os.path.dirname(root))
    return sorted(set(dirs))


def standard(project):
    path = os.path.join(project, 'CMakeLists.txt')
    with open(path, encoding='utf-8') as handle:
        match = re.search(r'set\(CMAKE_CXX_STANDARD\s+(\d+)', handle.read())
    return match.group(1) if match else '17'


def compiles(source, project, std, workdir):
    path = os.path.join(workdir, 'example.cpp')
    with open(path, 'w', encoding='utf-8') as handle:
        handle.write(source)
    command = ['g++', f'-std=c++{std}', '-fsyntax-only']
    command += [f'-I{path}' for path in include_dirs(project)]
    command.append(path)
    try:
        result = subprocess.run(command, capture_output=True, text=True,
                                timeout=60)
    except subprocess.TimeoutExpired:
        return False, 'the compiler did not finish'
    return result.returncode == 0, result.stderr


def shapes(code):
    """The two ways a fragment can be turned into a program."""
    lines = code.splitlines()
    includes = [l for l in lines if l.strip().startswith('#include')]
    rest = [l for l in lines if not l.strip().startswith('#include')]
    head = '\n'.join(includes)
    body = '\n'.join(rest)
    at_scope = head + '\n#include <iostream>\n' + body + '\n'
    in_main = (head + '\n#include <iostream>\nint main() {\n' + body +
               '\n    return 0;\n}\n')
    return [('at file scope', at_scope), ('inside a main()', in_main)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', nargs='?', default=REPO)
    args = parser.parse_args()
    root = os.path.abspath(args.root)
    roadmap = os.path.join(root, 'cpp-mastery-roadmap')

    problems = []
    checked = 0

    with tempfile.TemporaryDirectory() as workdir:
        for project in sorted(projects(roadmap)):
            readme = os.path.join(project, 'README.md')
            if not os.path.isfile(readme):
                continue
            with open(readme, encoding='utf-8') as handle:
                text = handle.read()
            blocks = [body for lang, body in code_blocks(text)
                      if lang in ('cpp', 'c++') and 'int main' not in body]
            if not blocks:
                continue
            checked += 1
            std = standard(project)
            rel = os.path.relpath(project, root)
            for index, code in enumerate(blocks):
                errors = []
                ok = False
                for label, source in shapes(code):
                    good, stderr = compiles(source, project, std, workdir)
                    if good:
                        ok = True
                        break
                    errors.append((label, stderr))
                if not ok:
                    label, stderr = errors[0]
                    detail = ''
                    for line in stderr.splitlines():
                        if ' error:' in line:
                            detail = line.strip()[:160]
                            break
                    problems.append(
                        f'{rel}: example {index + 1} does not compile ({detail})')

    print('== project README examples ==')
    print(f'  {checked} project(s) with a C++ example checked')
    if problems:
        print(f'\n{len(problems)} problem(s):')
        for problem in problems:
            print(f'  {problem}')
        return 1
    print('\nevery project README example compiles')
    return 0


if __name__ == '__main__':
    sys.exit(main())
