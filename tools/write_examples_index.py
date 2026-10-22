#!/usr/bin/env python3
"""Write the index of the examples/ directory.

The index is built from each example's own header comment, so the one-line
description is the same text the reader finds in the file itself. That way
the index cannot describe an example differently from the example's own
header.

Where an example clearly belongs to a project in the roadmap, the project is
linked as well.

Usage: python3 tools/write_examples_index.py
"""
import glob
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXAMPLES = os.path.join(REPO, 'examples')
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')

# The category an example falls into, by the kind of topic it covers.
def category(name):
    if name.startswith('pitfall-'):
        return ('Pitfalls', '陷阱')
    if re.match(r'(cpp20|cpp23|std-)', name) or name in (
            'constexpr', 'consteval', 'if-consteval', 'fold-expressions',
            'crtp-basics', 'deducing-this', 'inplace-vector', 'lambda-expressions',
            'nttp-class-type', 'perfect-forwarding', 'range-based-for',
            'spaceship-operator', 'structured-bindings', 'value-categories',
            'views-chunk-split', 'chrono-calendar', 'coroutine-basics'):
        return ('Modern C++ and the standard library', '现代 C++ 与标准库')
    if name.endswith('-demo'):
        return ('Pattern demos', '模式示例')
    return ('Core concepts', '核心概念')


def describe(path):
    """The one-line description from the example's own header."""
    text = open(path, encoding='utf-8', errors='replace').read()
    lines = text.splitlines()
    out = []
    for line in lines[1:]:
        if not line.startswith('//'):
            break
        if re.match(r'^//\s*Compile', line):
            break
        out.append(line[2:].strip())
    return ' '.join(out).rstrip()


def project_for(name):
    """The roadmap project an example belongs to, if there is an obvious one."""
    stem = name[:-4]
    for suffix in ('-demo', '-concurrency', '-benchmark', '-error-handling'):
        if stem.endswith(suffix):
            stem = stem[:-len(suffix)]
            break
    for project in sorted(glob.glob(os.path.join(ROADMAP, '*', '*'))):
        if not os.path.isfile(os.path.join(project, 'CMakeLists.txt')):
            continue
        if os.path.basename(project) == stem:
            return os.path.relpath(project, ROADMAP)
    return None


def main():
    sources = sorted(glob.glob(os.path.join(EXAMPLES, '*.cpp')))
    groups = {}
    for path in sources:
        name = os.path.basename(path)
        groups.setdefault(category(name), []).append(
            (name, describe(path), project_for(name)))

    for (en_title, zh_title), items in sorted(groups.items()):
        print(f'  {en_title}: {len(items)}')

    # Only the English index is generated. The Chinese one is translated by
    # hand and kept in the repository, so regenerating must not overwrite it.
    write_index(groups, lang='en')
    print('the Chinese index is translated by hand and left alone')
    return 0


def write_index(groups, lang):
    zh = lang == 'zh'
    out = []
    out.append('# Examples' if not zh else '# 示例')
    out.append('')
    if not zh:
        out.append('Every example is a single self-contained file. Each one states in')
        out.append('its own header what it demonstrates and how to build it.')
        out.append('')
        out.append('```bash')
        out.append('cd examples')
        out.append('g++ -std=c++20 -o demo demo-file.cpp && ./demo')
        out.append('```')
    else:
        out.append('每个示例都是一个自包含的单文件。文件头注释里写明了它演示什么、')
        out.append('以及怎么编译。')
        out.append('')
        out.append('```bash')
        out.append('cd examples')
        out.append('g++ -std=c++20 -o demo demo-file.cpp && ./demo')
        out.append('```')
    out.append('')

    for (en_title, zh_title), items in sorted(groups.items()):
        out.append(f'## {zh_title if zh else en_title}')
        out.append('')
        for name, description, project in items:
            out.append(f'- [{name}]({name}) — {description}')
            if project and not zh:
                # The index lives in examples/, so the project path is relative
                # to the repository root.
                out.append(f'  - part of [{os.path.basename(project)}]'
                           f'(../cpp-mastery-roadmap/{project}/README.md)')
        out.append('')

    path = os.path.join(EXAMPLES, 'README.zh-CN.md' if zh else 'README.md')
    open(path, 'w', encoding='utf-8').write('\n'.join(out))
    print(f'wrote {os.path.relpath(path, REPO)}')


if __name__ == '__main__':
    sys.exit(main())
