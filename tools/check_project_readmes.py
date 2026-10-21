#!/usr/bin/env python3
"""Check the project READMEs against their build and against each other.

Each project has an English README and a Chinese one. Three things can go
wrong, and all three have happened in this repository:

- the two halves drift apart, so the Chinese one describes an older revision
  of the project
- a README names a binary that the project does not build, so the documented
  command fails
- a README names a feature the project does not have

The first two are checked here. The third needs a human.

Usage: python3 tools/check_project_readmes.py [repo_root]
"""
import argparse
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')


def headings(text):
    """The heading levels of a markdown file, in order."""
    out = []
    fence = None
    for line in text.splitlines():
        m = re.match(r'^\s{0,3}(`{3,}|~{3,})', line)
        if fence is None:
            if m:
                fence = m.group(1)[0]
                continue
            h = re.match(r'^(#{1,4})\s+', line)
            if h:
                out.append(len(h.group(1)))
        elif m and m.group(1)[0] == fence:
            fence = None
    return out


def code_blocks(text):
    return re.findall(r'```(\w*)\n(.*?)```', text, re.S)


def chinese_lines(text):
    """Prose lines dominated by Chinese characters."""
    body = re.sub(r'```.*?```', '', text, flags=re.S)
    zh = en = 0
    for line in body.splitlines():
        cjk = len(re.findall(r'[\u4e00-\u9fff]', line))
        if cjk == 0:
            continue
        if cjk >= max(2, len(re.findall(r'[A-Za-z]+', line))):
            zh += 1
        else:
            en += 1
    return zh, en


def projects(roadmap=ROADMAP):
    for dirpath, dirnames, filenames in os.walk(roadmap):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            yield dirpath


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', nargs='?', default=REPO)
    args = parser.parse_args()
    root = os.path.abspath(args.root)
    roadmap = os.path.join(root, 'cpp-mastery-roadmap')

    problems = []
    checked = 0

    for project in sorted(projects(roadmap)):
        en_path = os.path.join(project, 'README.md')
        zh_path = os.path.join(project, 'README.zh-CN.md')
        if not (os.path.isfile(en_path) and os.path.isfile(zh_path)):
            continue
        checked += 1
        rel = os.path.relpath(project, root)

        en = open(en_path, encoding='utf-8').read()
        zh = open(zh_path, encoding='utf-8').read()

        en_h, zh_h = headings(en), headings(zh)
        if en_h != zh_h:
            problems.append(f'{rel}: the two READMEs have different heading '
                            f'skeletons ({len(en_h)} vs {len(zh_h)})')

        en_b, zh_b = code_blocks(en), code_blocks(zh)
        if len(en_b) != len(zh_b):
            problems.append(f'{rel}: the two READMEs have a different number '
                            f'of code blocks ({len(en_b)} vs {len(zh_b)})')

        zh_lines, en_lines = chinese_lines(zh)
        if zh_lines < en_lines:
            problems.append(f'{rel}: the Chinese README reads as English '
                            f'({zh_lines} chinese lines vs {en_lines} english)')

        # The binary a README tells the reader to run must be one the
        # project actually builds.
        cm = open(os.path.join(project, 'CMakeLists.txt'), encoding='utf-8').read()
        targets = set(re.findall(r'add_executable\s*\(\s*([A-Za-z0-9_]+)', cm))
        for readme, text in (('README.md', en), ('README.zh-CN.md', zh)):
            for m in re.finditer(r'\./build/([A-Za-z0-9_.-]+)', text):
                name = m.group(1)
                if name not in targets:
                    problems.append(
                        f'{rel}/{readme}: names ./build/{name}, which the '
                        f'project does not build ({", ".join(sorted(targets))})')

    print(f'== project READMEs ==')
    print(f'  {checked} project pairs checked')
    if problems:
        print(f'\n{len(problems)} problem(s):')
        for p in problems:
            print(f'  {p}')
        return 1
    print('\nevery project README agrees with its twin and with its build')
    return 0


if __name__ == '__main__':
    sys.exit(main())
