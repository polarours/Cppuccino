#!/usr/bin/env python3
"""Assemble a single-source MkDocs site from this repository.

No article is duplicated: the script copies the existing Markdown trees and
example sources into a temporary ``.site/`` directory, generates the index
pages and the nav, then writes a JSON (valid YAML) MkDocs config there.

Usage:
    python3 tools/build_site.py
    mkdocs build -f .site/mkdocs.yml --strict --site-dir ../site
"""
from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from collections import defaultdict
from pathlib import Path
from urllib.parse import quote

REPO = Path(__file__).resolve().parent.parent
TOOLS = Path(__file__).resolve().parent
SITE = REPO / '.site'
TEMPLATES = REPO / 'site-src'
REPO_URL = 'https://github.com/polarours/Cppuccino'
SITE_URL = 'https://polarours.github.io/Cppuccino/'

sys.path.insert(0, str(TOOLS))
import check_stats  # noqa: E402  (local helper, not a third-party package)


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.rstrip() + '\n', encoding='utf-8')


def copy_tracked(prefix: str) -> None:
    """Copy only tracked files, so local build/ directories stay out."""
    result = subprocess.run(['git', 'ls-files', prefix], cwd=REPO,
                            capture_output=True, text=True, check=True)
    for rel in result.stdout.splitlines():
        source = REPO / rel
        target = SITE / rel
        if source.is_dir():
            # A git submodule appears as a single gitlink entry.
            for path in source.rglob('*'):
                if not path.is_file():
                    continue
                if '.git' in path.parts or 'build' in path.parts:
                    continue
                dest = target / path.relative_to(source)
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(path, dest)
            continue
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)


def markdown_files(folder: str, lang: str) -> list[tuple[str, str]]:
    """Return (nav title, path relative to .site) for one article folder."""
    out = []
    for path in sorted((REPO / folder).glob('*.md')):
        title = path.stem
        if lang == 'en':
            title = title.replace('-', ' ').replace('_', ' ')
        out.append((title, f'{folder}/{path.name}'))
    return out


def example_rows() -> list[tuple[str, str]]:
    """Read the real one-line description each example puts at the top."""
    rows = []
    for path in sorted((REPO / 'examples').glob('*.cpp')):
        description = 'A self-contained C++ example.'
        for line in path.read_text(encoding='utf-8', errors='replace').splitlines()[:10]:
            text = line.strip()
            if not text.startswith('//'):
                continue
            text = text[2:].strip()
            if 'examples/' in text or text.startswith('Compile with:'):
                continue
            description = text
            break
        rows.append((path.name, description))
    return rows


def projects_by_stage() -> list[tuple[str, list[tuple[str, str]]]]:
    groups: dict[str, list[tuple[str, str]]] = defaultdict(list)
    for dirpath, dirnames, filenames in os.walk(REPO / 'cpp-mastery-roadmap'):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' not in filenames:
            continue
        rel = Path(dirpath).relative_to(REPO)
        stage = rel.parts[1] if len(rel.parts) > 1 else 'other'
        groups[stage].append((rel.name, rel.as_posix()))
    return sorted(groups.items())


def examples_page(lang: str) -> str:
    count = check_stats.count_files('examples/*.cpp')
    if lang == 'en':
        lines = [
            '# Examples',
            '',
            f'{count} runnable, self-contained files. Each states its own '
            'build command at the top; the site links to the real source.',
            '',
            '| Example | What it shows |',
            '|:--------|:--------------|',
        ]
        for name, description in example_rows():
            lines.append(f'| [{name}](../../examples/{name}) | {description} |')
        return '\n'.join(lines)

    lines = [
        '# 示例',
        '',
        f'{count} 个可独立运行的示例。每个文件开头都写了自己的编译命令；'
        '这里链接的是仓库里的真实源码。',
        '',
        '| 示例 | 演示内容 |',
        '|:-----|:---------|',
    ]
    for name, description in example_rows():
        lines.append(f'| [{name}](../../examples/{name}) | {description} |')
    return '\n'.join(lines)


def projects_page(lang: str) -> str:
    count = check_stats.count_projects()
    stages = check_stats.count_stages()
    if lang == 'en':
        lines = [
            '# Projects',
            '',
            f'{count} small projects across {stages} stages. Every project '
            'is built and tested in CI.',
            '',
        ]
        for stage, projects in projects_by_stage():
            lines.append(f'## {stage}')
            lines.append('')
            for name, rel in projects:
                url = f'{REPO_URL}/tree/master/{quote(rel)}'
                lines.append(f'- [{name}]({url})')
            lines.append('')
        return '\n'.join(lines)

    lines = [
        '# 项目',
        '',
        f'{count} 个小项目，横跨 {stages} 个阶段。每个项目都在 CI 里构建和测试。',
        '',
    ]
    for stage, projects in projects_by_stage():
        lines.append(f'## {stage}')
        lines.append('')
        for name, rel in projects:
            url = f'{REPO_URL}/tree/master/{quote(rel)}'
            lines.append(f'- [{name}]({url})')
        lines.append('')
    return '\n'.join(lines)


def build_config() -> dict:
    en_concepts = markdown_files('docs/en', 'en')
    zh_concepts = markdown_files('docs/中文版', 'zh')
    en_pitfalls = markdown_files('pitfalls/en', 'en')
    zh_pitfalls = markdown_files('pitfalls/中文版', 'zh')

    nav = [
        {'Home': 'index.md'},
        {'English': [
            {'Start here': 'en/index.md'},
            {'Concepts': [{title: path} for title, path in en_concepts]},
            {'Pitfalls': [{title: path} for title, path in en_pitfalls]},
            {'Examples': 'en/examples/index.md'},
            {'Projects': 'en/projects/index.md'},
        ]},
        {'中文': [
            {'从这里开始': 'zh/index.md'},
            {'概念文档': [{title: path} for title, path in zh_concepts]},
            {'陷阱文章': [{title: path} for title, path in zh_pitfalls]},
            {'示例': 'zh/examples/index.md'},
            {'项目': 'zh/projects/index.md'},
        ]},
        {'Tooling': 'tooling/index.md'},
    ]

    return {
        'site_name': 'Cppuccino — bilingual C++ notebook',
        'site_description': 'One C++ idea, in two languages, with runnable proof.',
        'site_url': SITE_URL,
        'repo_url': REPO_URL,
        'repo_name': 'polarours/Cppuccino',
        'docs_dir': '.',
        'nav': nav,
        'theme': {
            'name': 'material',
            'language': 'en',
            'palette': [
                {
                    'media': '(prefers-color-scheme: light)',
                    'scheme': 'default',
                    'primary': 'brown',
                    'accent': 'amber',
                    'toggle': {
                        'icon': 'material/brightness-7',
                        'name': 'Switch to dark mode',
                    },
                },
                {
                    'media': '(prefers-color-scheme: dark)',
                    'scheme': 'slate',
                    'primary': 'brown',
                    'accent': 'amber',
                    'toggle': {
                        'icon': 'material/brightness-4',
                        'name': 'Switch to light mode',
                    },
                },
            ],
            'font': {'text': 'Inter', 'code': 'JetBrains Mono'},
            'features': [
                'navigation.instant',
                'navigation.tracking',
                'navigation.top',
                'search.suggest',
                'search.highlight',
                'content.code.copy',
                'content.tabs.link',
            ],
        },
        'plugins': ['search'],
        'markdown_extensions': [
            'admonition',
            'attr_list',
            'def_list',
            'footnotes',
            'tables',
            {'toc': {'permalink': True}},
            'pymdownx.details',
            'pymdownx.superfences',
            'pymdownx.inlinehilite',
            {'pymdownx.highlight': {'anchor_linenums': True}},
            {'pymdownx.tabbed': {'alternate_style': True}},
        ],
        'extra_css': ['assets/stylesheets/extra.css'],
        'validation': {
            'links': {'not_found': 'warn'},
            'anchors': 'info',
        },
        'extra': {
            'social': [
                {
                    'icon': 'fontawesome/brands/github',
                    'link': REPO_URL,
                    'name': 'Cppuccino on GitHub',
                }
            ]
        },
    }


def stats_tokens() -> dict[str, str]:
    return {
        'DOCS': str(check_stats.count_files('docs/en/*.md') +
                    check_stats.count_files('docs/中文版/*.md')),
        'PITFALLS': str(check_stats.count_files('pitfalls/en/*.md') +
                        check_stats.count_files('pitfalls/中文版/*.md')),
        'EXAMPLES': str(check_stats.count_files('examples/*.cpp')),
        'PROJECTS': str(check_stats.count_projects()),
        'STAGES': str(check_stats.count_stages()),
        'TESTS': str(check_stats.count_tests()),
    }


def main() -> int:
    if SITE.exists():
        shutil.rmtree(SITE)
    SITE.mkdir(parents=True)

    shutil.copytree(TEMPLATES, SITE, dirs_exist_ok=True)

    for folder in ('docs/en', 'docs/中文版', 'pitfalls/en', 'pitfalls/中文版'):
        target = SITE / folder
        target.mkdir(parents=True, exist_ok=True)
        for path in sorted((REPO / folder).glob('*.md')):
            shutil.copy2(path, target / path.name)

    examples = SITE / 'examples'
    examples.mkdir(parents=True, exist_ok=True)
    for path in sorted((REPO / 'examples').glob('*.cpp')):
        shutil.copy2(path, examples / path.name)

    images = SITE / 'assets' / 'images'
    images.mkdir(parents=True, exist_ok=True)
    for name in ('cppuccino-banner-light.svg', 'cppuccino-banner-dark.svg',
                 'value-categories.png'):
        shutil.copy2(REPO / 'assets' / 'images' / name, images / name)

    # Concept articles link into the roadmap and tinyshell with relative
    # paths. Copying tracked files keeps those links real without dragging
    # local build/ directories into the site.
    copy_tracked('cpp-mastery-roadmap')
    copy_tracked('tinyshell')

    write(SITE / 'en' / 'examples' / 'index.md', examples_page('en'))
    write(SITE / 'en' / 'projects' / 'index.md', projects_page('en'))
    write(SITE / 'zh' / 'examples' / 'index.md', examples_page('zh'))
    write(SITE / 'zh' / 'projects' / 'index.md', projects_page('zh'))

    # Landing pages use {{TOKEN}} placeholders so their numbers cannot drift
    # away from the README and the repository.
    tokens = stats_tokens()
    for path in SITE.rglob('*.md'):
        text = path.read_text(encoding='utf-8')
        for key, value in tokens.items():
            text = text.replace('{{' + key + '}}', value)
        path.write_text(text, encoding='utf-8')

    # JSON is a subset of YAML, so MkDocs reads this without a YAML dependency.
    config = json.dumps(build_config(), indent=2, ensure_ascii=False)
    write(SITE / 'mkdocs.yml', config)

    print(f'prepared {SITE.relative_to(REPO)}: '
          f'{check_stats.count_files("docs/en/*.md") + check_stats.count_files("docs/中文版/*.md")} docs, '
          f'{check_stats.count_files("pitfalls/en/*.md") + check_stats.count_files("pitfalls/中文版/*.md")} pitfalls, '
          f'{check_stats.count_files("examples/*.cpp")} examples, '
          f'{check_stats.count_projects()} projects')
    return 0


if __name__ == '__main__':
    sys.exit(main())
