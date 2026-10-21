#!/usr/bin/env python3
"""Check that the statistics in the READMEs match the repository.

The numbers in the READMEs drifted more than once while the repository was
growing. This script recomputes each figure and compares it with what the
README claims, so a mismatch is a failed check rather than a stale number
somebody has to notice by hand.

Usage: python3 tools/check_stats.py [repo_root]
"""
import glob
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def count_files(pattern):
    return len(glob.glob(os.path.join(REPO, pattern)))


def count_projects():
    total = 0
    for dirpath, dirnames, filenames in os.walk(os.path.join(REPO, 'cpp-mastery-roadmap')):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            total += 1
    return total


def count_stages():
    stages = 0
    for name in os.listdir(os.path.join(REPO, 'cpp-mastery-roadmap')):
        path = os.path.join(REPO, 'cpp-mastery-roadmap', name)
        if os.path.isdir(path) and name[0].isdigit():
            stages += 1
    return stages


def count_tests():
    pattern = os.path.join(REPO, 'cpp-mastery-roadmap', '**', '*.cpp')
    total = 0
    for path in glob.glob(pattern, recursive=True):
        if os.sep + 'build' + os.sep in path:
            continue
        source = open(path, encoding='utf-8', errors='replace').read()
        total += len(re.findall(r'\bvoid\s+test_[A-Za-z0-9_]*\s*\(', source))
    return total


def read(path):
    return open(os.path.join(REPO, path), encoding='utf-8').read()


def main():
    docs_en = count_files('docs/en/*.md')
    docs_zh = count_files('docs/中文版/*.md')
    pit_en = count_files('pitfalls/en/*.md')
    pit_zh = count_files('pitfalls/中文版/*.md')
    examples = count_files('examples/*.cpp')
    projects = count_projects()
    stages = count_stages()
    tests = count_tests()

    en = read('README.md')
    zh = read('README_zh-CN.md')

    problems = []

    def want(label, pattern, expected, text, name, group=1):
        found = re.search(pattern, text, re.M)
        if not found:
            problems.append(f'{name}: no line matching {pattern!r} for {label}')
            return
        if int(found.group(group)) != expected:
            problems.append(
                f'{name}: {label} says {found.group(group)}, '
                f'the repository has {expected}')

    # The English README
    want('documentation', r'\| Documentation \| (\d+) articles', docs_en + docs_zh, en, 'README.md')
    want('pitfalls', r'\| Pitfalls \| (\d+) articles', pit_en + pit_zh, en, 'README.md')
    want('examples', r'\| Examples \| (\d+) runnable examples', examples, en, 'README.md')
    want('projects', r'\| Projects \| (\d+) projects across (\d+) stages', projects, en, 'README.md')
    want('stages', r'\| Projects \| (\d+) projects across (\d+) stages', stages, en, 'README.md', group=2)
    want('tests', r'\| Tests \| (\d+) \(all passing\)', tests, en, 'README.md')

    # The Chinese README
    want('documentation', r'\| 文档 \| (\d+) 篇', docs_en + docs_zh, zh, 'README_zh-CN.md')
    want('pitfalls', r'\| 陷阱 \| (\d+) 篇', pit_en + pit_zh, zh, 'README_zh-CN.md')
    want('examples', r'\| 示例 \| (\d+) 个可运行示例', examples, zh, 'README_zh-CN.md')
    want('projects', r'\| 项目 \| (\d+) 个项目，横跨 (\d+) 个阶段', projects, zh, 'README_zh-CN.md')
    want('stages', r'\| 项目 \| (\d+) 个项目，横跨 (\d+) 个阶段', stages, zh, 'README_zh-CN.md', group=2)
    want('tests', r'\| 测试 \| (\d+)（全部通过）', tests, zh, 'README_zh-CN.md')

    print('== statistics ==')
    print(f'  documentation: {docs_en} en + {docs_zh} zh = {docs_en + docs_zh}')
    print(f'  pitfalls:     {pit_en} en + {pit_zh} zh = {pit_en + pit_zh}')
    print(f'  examples:     {examples}')
    print(f'  projects:     {projects} across {stages} stages')
    print(f'  tests:        {tests}')

    if problems:
        print(f'\n{len(problems)} mismatch(es):')
        for p in problems:
            print(f'  {p}')
        return 1
    print('\nthe READMEs agree with the repository')
    return 0


if __name__ == '__main__':
    sys.exit(main())
