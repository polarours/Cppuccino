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
import re
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

WORLDS = [
    {
        'key': 'ownership',
        'en': 'Ownership & Lifetime',
        'zh': '所有权与生命周期',
        'tagline_en': 'Who owns it, and when does it die?',
        'tagline_zh': '谁拥有它，它什么时候结束？',
        'keywords': ['ownership', 'lifetime', 'raii', 'move', 'dangling',
                     'use-after-move', 'unique_ptr', 'shared_ptr',
                     'reference', 'rvalue', 'lvalue', 'value-categor',
                     'forwarding', 'copy elision', 'rule of zero',
                     'object model', 'const',
                     '所有权', '生命周期', '悬垂', '移动', '智能指针',
                     '引用', '右值', '左值', '转发', '拷贝', '零三五',
                     '对象模型'],
    },
    {
        'key': 'patterns',
        'en': 'Design Patterns',
        'zh': '设计模式',
        'tagline_en': 'Names for recurring shapes of code.',
        'tagline_zh': '给反复出现的代码形状起名字。',
        'keywords': ['pattern', 'factory', 'strategy', 'observer', 'command',
                     'visitor', 'decorator', 'singleton', 'dispatch', 'pimpl',
                     'abi', '模式', '策略', '命令', '观察者', '分派', '双重分派'],
    },
    {
        'key': 'memory',
        'en': 'Memory & Allocators',
        'zh': '内存与分配器',
        'tagline_en': 'Every byte has an address and a cost.',
        'tagline_zh': '每个字节都有地址和代价。',
        'keywords': ['allocat', 'memory', 'arena', 'slab', 'pool', 'object-pool',
                     'allocation', 'double free', 'delete', 'free',
                     '内存', '分配', '对象池', '内存池', '重复释放'],
    },
    {
        'key': 'systems',
        'en': 'Systems & Concurrency',
        'zh': '系统与并发',
        'tagline_en': 'Processes, signals, threads, and shared state.',
        'tagline_zh': '进程、信号、线程与共享状态。',
        'keywords': ['thread', 'mutex', 'atomic', 'signal', 'process', 'shell',
                     'concurrency', 'lock', 'data race', 'simd', 'scheduler',
                     'server', 'iterator',
                     '线程', '并发', '信号', '原子', '内存序',
                     '互斥', '数据竞争', '调度', '服务器', '迭代器'],
    },
    {
        'key': 'modern',
        'en': 'Modern C++',
        'zh': '现代 C++',
        'tagline_en': 'What C++20 and C++23 actually changed.',
        'tagline_zh': 'C++20 与 C++23 到底改变了什么。',
        'keywords': ['cpp20', 'cpp23', 'concept', 'coroutine', 'range', 'print',
                     'mdspan', 'span', 'optional', 'expected', 'generator',
                     'template', 'fold expression', 'two phase',
                     '概念', '协程', '范围', '模板', '折叠', '两阶段'],
    },
    {
        'key': 'craft',
        'en': 'Craft & Tooling',
        'zh': '工程与工具',
        'tagline_en': 'Build it, test it, document it, keep it honest.',
        'tagline_zh': '构建、测试、写文档，让它保持诚实。',
        'keywords': ['cmake', 'test', 'ci', 'doc', 'parser', 'link', 'diff',
                     'build', 'markdown', '测试', '构建', '文档', '工具'],
    },
]

NAV_GROUPS = [
    {
        'key': 'modern',
        'en': 'Modern C++',
        'zh': '现代 C++',
        'keywords': ['cpp20', 'cpp23', 'concept', 'ranges', 'std-expected',
                     'expected', 'auto', 'decltype', 'coroutine', 'format',
                     'span', 'optional', 'mdspan', 'template-instantiation',
                     'two-phase', '概念', '范围', '协程', '自动', '推导',
                     '模板实例化', '两阶段'],
    },
    {
        'key': 'craft',
        'en': 'Craft & Tooling',
        'zh': '工程与工具',
        'keywords': ['cmake', 'build-systems', 'testing', 'test', 'glossary',
                     'tooling', '构建系统', '测试', '术语', '工具'],
    },
    {
        'key': 'architecture',
        'en': 'Architecture & Enterprise',
        'zh': '架构与企业模式',
        'keywords': ['active-record', 'cqrs', 'event-sourcing', 'mvc',
                     'pimpl', 'repository', 'service-locator', 'specification',
                     'unit-of-work', '活动记录', '事件溯源', '仓储',
                     '服务定位器', '规范', '单元工作', '架构'],
    },
    {
        'key': 'design',
        'en': 'Design Patterns',
        'zh': '设计模式',
        'keywords': ['pattern', 'factory', 'adapter', 'bridge', 'builder',
                     'chain-of', 'command', 'composite', 'decorator',
                     'delegation', 'double-dispatch', 'facade', 'flyweight',
                     'interpreter', 'mediator', 'memento', 'null-object',
                     'observer', 'prototype', 'proxy', 'singleton',
                     'strategy', 'template-method', 'type-object', 'visitor',
                     'balking', 'iterator-pattern', '模式', '工厂', '适配器',
                     '桥接', '建造者', '责任链', '命令', '组合', '装饰',
                     '委托', '双重分派', '外观', '享元', '解释器', '中介者',
                     '备忘录', '空对象', '观察者', '原型', '代理', '单例',
                     '策略', '模板方法', '类型对象', '访问者', '推辞'],
    },
    {
        'key': 'foundations',
        'en': 'Foundations',
        'zh': '基础',
        'keywords': ['ownership', 'lifetime', 'raii', 'rule-of', 'copy-elision',
                     'value-categor', 'lvalue', 'rvalue', 'reference', 'move',
                     'forwarding', 'const', 'object-model', 'virtual-dispatch',
                     'noexcept', 'undefined-behavior', '所有权', '生命周期',
                     '零三五', '拷贝消除', '值类别', '左值', '右值', '引用',
                     '移动', '转发', '对象模型', '虚函数', '未定义行为'],
    },
    {
        'key': 'systems',
        'en': 'Systems & Performance',
        'zh': '系统与性能',
        'keywords': ['chat-server', 'connection-pool', 'memory-pool',
                     'allocator', 'simd', 'performance', 'database', 'db-api',
                     'scheduler', 'memory-model', 'atomics', 'concurrency',
                     'thread', 'server', 'api', '内存池', '分配器', '性能',
                     '数据库', '调度', '内存模型', '原子', '并发', '线程',
                     '服务器'],
    },
]

NAV_DISPLAY_ORDER = ['foundations', 'modern', 'design', 'architecture',
                     'systems', 'craft']

# A few pages are the intended on-ramp, not alphabetical entries. Keep them
# first inside their group without inventing a second ordering system.
NAV_PINS = {
    'docs/en': [
        'ownership-starts-here.md',
        'ownership-and-lifetime.md',
        'raii-and-exception-safety.md',
    ],
    'docs/中文版': [
        '所有权-从这里开始.md',
        '所有权与生命周期.md',
        'RAII与异常安全.md',
    ],
}

PITFALL_GROUPS = [
    {
        'key': 'concurrency',
        'en': 'Concurrency',
        'zh': '并发',
        'keywords': ['data-race', 'deadlock', 'livelock', 'priority-inversion',
                     'memory-order', 'thread', 'double-checked', 'thread-safety',
                     '数据竞态', '死锁', '活锁', '优先级反转', '内存序',
                     '线程', '双重检查', '线程安全'],
    },
    {
        'key': 'iterators',
        'en': 'Containers & Iterators',
        'zh': '容器与迭代器',
        'keywords': ['iterator-invalidation', '迭代器失效'],
    },
    {
        'key': 'templates',
        'en': 'Templates & Language',
        'zh': '模板与语言',
        'keywords': ['fold-expression', 'implicit-conversions', 'macro',
                     'narrowing', 'sfinae', 'signed-integer-overflow',
                     'stack-overflow', 'template-deduction',
                     'template-specialization', '折叠', '隐式转换', '宏',
                     '窄化', '有符号整数', '栈溢出', '模板参数推导',
                     '模板特化'],
    },
    {
        'key': 'lifetime',
        'en': 'Lifetime & Ownership',
        'zh': '生命周期与所有权',
        'keywords': ['dangling', 'double-free', 'memory-leak', 'const',
                     'moving-from', 'object-lifetime', 'object-slicing',
                     'returning-reference', 'throwing-from-destructor',
                     'uninitialized', 'use-after-move', 'static-initialization',
                     'deleting-through', 'memory-fragmentation', '悬垂',
                     '双重释放', '内存泄漏', '对象生命周期', '对象切片',
                     '返回局部', '析构', '未初始化', '移动后', '静态初始化',
                     '非虚基类', '内存碎片'],
    },
]

sys.path.insert(0, str(TOOLS))
import check_stats  # noqa: E402  (local helper, not a third-party package)
from check_links import strip_code  # noqa: E402


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


def first_heading(path: Path) -> str:
    for line in path.read_text(encoding='utf-8', errors='replace').splitlines():
        match = re.match(r'^#\s+(.+?)\s*#*\s*$', line)
        if match:
            return match.group(1)
    return ''


def clean_nav_title(raw: str, fallback: str) -> str:
    """Turn a page H1 into a short, human nav label."""
    title = re.sub(r'`([^`]*)`', r'\1', raw).strip()
    # Drop a trailing duplicate gloss such as "(C++20 Concepts)" or
    # "（Object Slicing）"; keep tokens such as decltype(auto).
    title = re.sub(r'\s*[\(（](?=[^()（）]*\s)[^()（）]*[\)）]\s*$', '', title)
    # "Undefined Behavior — Hidden Traps" reads better in a sidebar than the
    # full subtitle. The Chinese long dash is handled too.
    title = re.split(r'\s+[—–]\s+|——', title, maxsplit=1)[0]
    title = re.sub(r'\s+', ' ', title).strip()
    return title or fallback


def nav_group_key(title: str, path: str,
                  groups: list[dict] = NAV_GROUPS) -> str:
    text = f'{title} {path}'.lower()
    for group in groups:
        if any(word in text for word in group['keywords']):
            return group['key']
    return groups[-1]['key']


def nav_group_label(key: str, lang: str,
                    groups: list[dict] = NAV_GROUPS) -> str:
    for group in groups:
        if group['key'] == key:
            return group[lang]
    return key


def markdown_files(folder: str, lang: str,
                   use_heading: bool = True) -> list[tuple[str, str]]:
    """Return (nav title, path relative to .site) for one article folder."""
    out = []
    pins = NAV_PINS.get(folder, [])
    pin_order = {name: index for index, name in enumerate(pins)}
    paths = sorted(
        (REPO / folder).glob('*.md'),
        key=lambda path: (pin_order.get(path.name, len(pins)), path.name),
    )
    for path in paths:
        fallback = path.stem
        if lang == 'en':
            fallback = fallback.replace('-', ' ').replace('_', ' ')
        title = (clean_nav_title(first_heading(path), fallback)
                 if use_heading else fallback)
        out.append((title, f'{folder}/{path.name}'))
    return out


def grouped_markdown(folder: str, lang: str,
                     groups: list[dict] = NAV_GROUPS,
                     order: list[str] | None = None
                     ) -> list[tuple[str, list[tuple[str, str]]]]:
    grouped: dict[str, list[tuple[str, str]]] = defaultdict(list)
    for title, path in markdown_files(folder, lang):
        grouped[nav_group_key(title, path, groups)].append((title, path))
    if order:
        by_key = {group['key']: group for group in groups}
        ordered = [by_key[key] for key in order if key in by_key]
    else:
        ordered = groups
    return [
        (nav_group_label(group['key'], lang, groups), grouped[group['key']])
        for group in ordered
        if grouped[group['key']]
    ]


def nav_tree(folder: str, lang: str, all_label: str,
             groups: list[dict] = NAV_GROUPS,
             order: list[str] | None = None) -> list[dict]:
    tree: list[dict] = [{all_label: f'{folder}/index.md'}]
    for label, items in grouped_markdown(folder, lang, groups, order):
        tree.append({label: [{title: path} for title, path in items]})
    return tree


def article_index(folder: str, heading: str, intro: str, lang: str,
                  groups: list[dict] = NAV_GROUPS,
                  order: list[str] | None = None) -> str:
    """A real index page so /docs/en/ and friends do not 404."""
    lines = [f'# {heading}', '', intro, '']
    for label, items in grouped_markdown(folder, lang, groups, order):
        lines.append(f'## {label}')
        lines.append('')
        for title, path in items:
            name = path.rsplit('/', 1)[-1]
            lines.append(f'- [{title}]({name})')
        lines.append('')
    return '\n'.join(lines)


def strip_inline_markdown(text: str) -> str:
    text = re.sub(r'!\[[^\]]*\]\([^)]*\)', '', text)
    text = re.sub(r'\[([^\]]+)\]\([^)]*\)', r'\1', text)
    text = re.sub(r'`([^`]*)`', r'\1', text)
    text = re.sub(r'<[^>]+>', '', text)
    text = re.sub(r'[*_~]+', '', text)
    return re.sub(r'\s+', ' ', text).strip()


def markdown_description(text: str, fallback: str) -> str:
    """Take the first readable paragraph after the H1, within SEO length."""
    if text.startswith('---\n'):
        return fallback
    lines = text.splitlines()
    start = 0
    title = ''
    for index, line in enumerate(lines):
        match = re.match(r'^#\s+(.+?)\s*#*\s*$', line)
        if match:
            start = index + 1
            title = clean_nav_title(match.group(1), '')
            break
    if not title:
        return fallback

    paragraph: list[str] = []
    for line in lines[start:]:
        stripped = line.strip()
        if not stripped:
            if paragraph:
                break
            continue
        if stripped.startswith('```'):
            break
        if stripped.startswith(('#', '|', '---')):
            continue
        if not paragraph and re.match(r'^([-*+]|\d+\.)\s', stripped):
            continue
        if stripped.startswith('>'):
            stripped = stripped.lstrip('>').strip()
        if stripped:
            paragraph.append(stripped)

    description = strip_inline_markdown(' '.join(paragraph))
    if not description:
        return fallback
    if len(description) < 40:
        description = f'{title}: {description}'
    if len(description) > 158:
        shortened = description[:155].rsplit(' ', 1)[0].rstrip(' ,.;:，。；：')
        description = shortened + '…'
    return description


def add_descriptions(fallback: str) -> None:
    """Add page-level meta descriptions without touching the source files."""
    for path in SITE.rglob('*.md'):
        text = path.read_text(encoding='utf-8', errors='replace')
        if text.startswith('---\n'):
            continue
        description = markdown_description(text, fallback)
        # JSON strings are valid YAML double-quoted scalars and escape the
        # colon/quote combinations that would make a plain scalar ambiguous.
        description = re.sub(r'\s+', ' ', description).strip()
        front = ('---\ndescription: ' + json.dumps(description, ensure_ascii=False)
                 + '\n---\n\n')
        path.write_text(front + text, encoding='utf-8')


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


def directory_link_targets():
    """Collect every internal trailing-slash link in the assembled site."""
    targets = set()
    for path in SITE.rglob('*.md'):
        text = strip_code(path.read_text(encoding='utf-8', errors='replace'))
        for match in re.finditer(r'\]\(([^)\s]+)\)', text):
            link = match.group(1)
            if link.startswith(('http://', 'https://', 'mailto:', '#')):
                continue
            raw = link.partition('#')[0]
            if not raw.endswith('/'):
                continue
            resolved = (path.parent / raw).resolve()
            if SITE in resolved.parents:
                targets.add(resolved)
    return targets


def normalize_directory_links() -> None:
    """Point directory links at the README MkDocs already publishes there."""
    for path in SITE.rglob('*.md'):
        text = path.read_text(encoding='utf-8', errors='replace')
        changed = False

        def rewrite(match):
            nonlocal changed
            link = match.group(1)
            if link.startswith(('http://', 'https://', 'mailto:', '#')):
                return match.group(0)
            raw, sep, anchor = link.partition('#')
            if not raw.endswith('/'):
                return match.group(0)
            directory = (path.parent / raw).resolve()
            if SITE not in directory.parents or not (directory / 'README.md').is_file():
                return match.group(0)
            changed = True
            tail = 'README.md' + ('#' + anchor if sep else '')
            return f']({raw}{tail})'

        updated = re.sub(r'\]\(([^)\s]+)\)', rewrite, text)
        if changed:
            path.write_text(updated, encoding='utf-8')


def ensure_directory_indexes() -> None:
    """Give every linked directory a page instead of a 404."""
    for directory in sorted(directory_link_targets()):
        index = directory / 'index.md'
        if index.is_file() or (directory / 'README.md').is_file() or \
           (directory / 'index.html').is_file():
            continue
        rel = directory.relative_to(SITE).as_posix()
        github = f'{REPO_URL}/tree/master/{quote(rel)}'
        write(index, f'# {directory.name}\n\n'
                     f'This directory is part of the repository: '
                     f'[view it on GitHub]({github}).\n')


def validate_inline_labs() -> None:
    """Embedded lab src paths must resolve from the article's own directory."""
    broken = []
    pattern = re.compile(r'data-inline-lab[^>]*?data-src="([^"]+)"')
    for path in SITE.rglob('*.md'):
        text = path.read_text(encoding='utf-8', errors='replace')
        for match in pattern.finditer(text):
            raw = match.group(1).split('?')[0]
            # MkDocs serves foo.md at /foo/, one level deeper than the file.
            base = path.parent / path.stem
            resolved = (base / raw).resolve()
            if not (resolved / 'index.html').is_file() and \
               not (resolved / 'index.md').is_file():
                broken.append(f'{path.relative_to(SITE)} -> {raw}')
    if broken:
        raise SystemExit('broken inline lab sources:\n  ' + '\n  '.join(broken))


def validate_html_assets() -> None:
    """Standalone pages need their CSS/JS/links to exist at their real depth."""
    broken = []
    for path in SITE.rglob('*.html'):
        text = path.read_text(encoding='utf-8', errors='replace')
        for match in re.finditer(r'(?:href|src)="([^"]+)"', text):
            target = match.group(1)
            if target.startswith(('http://', 'https://', 'mailto:', '#', '//',
                                  'data:')):
                continue
            raw = target.partition('#')[0].partition('?')[0]
            if not raw:
                continue
            resolved = (path.parent / raw).resolve()
            if not resolved.exists() and raw.endswith('/'):
                # MkDocs turns foo.md into /foo/ with directory URLs.
                markdown = Path(str(resolved).rstrip('/') + '.md')
                if markdown.is_file():
                    continue
            if not resolved.exists():
                broken.append(f'{path.relative_to(SITE)} -> {target}')
            elif resolved.is_dir() and not (
                    (resolved / 'index.html').is_file() or
                    (resolved / 'index.md').is_file()):
                broken.append(f'{path.relative_to(SITE)} -> {target} (no index)')
    if broken:
        raise SystemExit('broken standalone-page links:\n  ' +
                         '\n  '.join(broken))


def validate_directory_links() -> None:
    """A link ending in '/' needs a real index page, or the site 404s."""
    broken = []
    for path in SITE.rglob('*.md'):
        text = strip_code(path.read_text(encoding='utf-8', errors='replace'))
        for match in re.finditer(r'\]\(([^)\s]+)\)', text):
            target = match.group(1)
            if target.startswith(('http://', 'https://', 'mailto:', '#')):
                continue
            raw = target.partition('#')[0]
            if not raw.endswith('/'):
                continue
            resolved = (path.parent / raw).resolve()
            if not ((resolved / 'index.md').is_file() or
                    (resolved / 'index.html').is_file()):
                broken.append(f'{path.relative_to(SITE)} -> {target}')
    if broken:
        raise SystemExit('directory links without an index page:\n  ' +
                         '\n  '.join(broken))


def classify(text: str) -> str:
    lowered = text.lower()
    for world in WORLDS:
        if any(keyword in lowered for keyword in world['keywords']):
            return world['key']
    return 'craft'


def quest(kind: str, lang: str, title: str, url: str, xp: int,
          source: str, description: str = '') -> dict:
    return {
        'id': f'{kind}-{lang}-{source}'.replace('/', '-').replace(' ', '-'),
        'kind': kind,
        'lang': lang,
        'title': title,
        'url': url,
        'xp': xp,
        'world': classify(title + ' ' + source),
        'description': description,
    }


def build_quests() -> dict:
    """Turn real repository content into game levels, without duplicating it."""
    quests = []

    for title, path in markdown_files('docs/en', 'en', use_heading=False):
        stem = path.rsplit('/', 1)[-1][:-3]
        quests.append(quest('intel', 'en', title,
                            '../docs/en/' + quote(stem) + '/', 10, path))

    for title, path in markdown_files('docs/中文版', 'zh', use_heading=False):
        stem = path.rsplit('/', 1)[-1][:-3]
        quests.append(quest('intel', 'zh', title,
                            '../docs/中文版/' + quote(stem) + '/', 10, path))

    for title, path in markdown_files('pitfalls/en', 'en', use_heading=False):
        stem = path.rsplit('/', 1)[-1][:-3]
        quests.append(quest('boss', 'en', title,
                            '../pitfalls/en/' + quote(stem) + '/', 20, path))

    for title, path in markdown_files('pitfalls/中文版', 'zh', use_heading=False):
        stem = path.rsplit('/', 1)[-1][:-3]
        quests.append(quest('boss', 'zh', title,
                            '../pitfalls/中文版/' + quote(stem) + '/', 20, path))

    for name, description in example_rows():
        quests.append(quest('loot', 'en', name[:-4].replace('-', ' '),
                            '../examples/' + quote(name), 15,
                            'examples/' + name, description))

    quests.append(quest('challenge', 'en', 'Dangling Pointer Boss',
                        '../arcade/boss/', 40, 'boss/dangling-pointer'))
    quests.append(quest('challenge', 'en', 'Iterator Invalidation Boss',
                        '../arcade/boss/iterator-invalidation/', 40,
                        'boss/iterator-invalidation'))
    quests.append(quest('challenge', 'en', 'Data Race Boss',
                        '../arcade/boss/data-race/', 40, 'boss/data-race'))
    quests.append(quest('challenge', 'en', 'Double Free Boss',
                        '../arcade/boss/double-free/', 40, 'boss/double-free'))

    exercises = REPO / 'tinyshell' / 'tests' / 'exercises'
    if exercises.is_dir():
        for path in sorted(exercises.glob('*.cpp')):
            name = path.name
            url = (f'https://github.com/polarours/tinyshell/blob/v1/'
                   f'tests/exercises/{quote(name)}')
            quests.append(quest('challenge', 'en',
                                path.stem.replace('_', ' ').replace('-', ' '),
                                url, 30, 'tinyshell/tests/exercises/' + name))

    return {
        'worlds': [{k: v for k, v in world.items() if k != 'keywords'}
                   for world in WORLDS],
        'quests': quests,
        'legend': {
            'intel': {'en': 'Read', 'zh': '阅读'},
            'boss': {'en': 'Boss', 'zh': 'Boss'},
            'loot': {'en': 'Loot', 'zh': '掉落'},
            'challenge': {'en': 'Clear', 'zh': '通关'},
        },
    }


def build_config() -> dict:
    nav = [
        {'Home': 'index.md'},
        {'English': [
            {'Start here': 'en/index.md'},
            {'Concepts': nav_tree('docs/en', 'en', 'All concepts',
                                  order=NAV_DISPLAY_ORDER)},
            {'Pitfalls': nav_tree('pitfalls/en', 'en', 'All pitfalls',
                                  PITFALL_GROUPS)},
            {'Examples': 'en/examples/index.md'},
            {'Projects': 'en/projects/index.md'},
        ]},
        {'中文': [
            {'从这里开始': 'zh/index.md'},
            {'概念文档': nav_tree('docs/中文版', 'zh', '全部概念',
                                  order=NAV_DISPLAY_ORDER)},
            {'陷阱文章': nav_tree('pitfalls/中文版', 'zh', '全部陷阱',
                                  PITFALL_GROUPS)},
            {'示例': 'zh/examples/index.md'},
            {'项目': 'zh/projects/index.md'},
        ]},
        {'Play': [
            {'Arcade': SITE_URL + 'arcade/'},
            {'UB Camp': SITE_URL + 'arcade/camp/'},
            {'Proof Gate': SITE_URL + 'arcade/camp/prove/'},
            {'Interactive Labs': SITE_URL + 'lab/'},
        ]},
        {'Glossary': 'docs/i18n/terminology.md'},
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
            'custom_dir': '../site-src/overrides',
            'palette': {
                'scheme': 'default',
                'primary': 'brown',
                'accent': 'amber',
            },
            'font': False,
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
            'meta',
            'tables',
            {'toc': {'permalink': True}},
            'pymdownx.details',
            'pymdownx.superfences',
            'pymdownx.inlinehilite',
            {'pymdownx.highlight': {'anchor_linenums': True}},
            {'pymdownx.tabbed': {'alternate_style': True}},
        ],
        'extra_css': [
            'assets/fonts.css',
            'assets/stylesheets/extra.css',
            'assets/read-theme.css',
            'assets/inline-lab.css',
        ],
        'extra_javascript': [
            'assets/read-theme.js',
            'assets/inline-lab.js',
            'assets/lang.js',
        ],
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


def validate_template_tokens(tokens: dict[str, str]) -> None:
    """A missing brace must fail the build, not ship as visible text."""
    keys = '|'.join(re.escape(key) for key in tokens)
    leftover = re.compile(r'\{\{(?:' + keys + r')\}(?!\})')
    broken = []
    for path in SITE.rglob('*.md'):
        text = path.read_text(encoding='utf-8', errors='replace')
        for match in leftover.finditer(text):
            broken.append(f'{path.relative_to(SITE)} -> {match.group(0)}')
    if broken:
        raise SystemExit('unresolved template tokens:\n  ' +
                         '\n  '.join(broken))


def main() -> int:
    if SITE.exists():
        shutil.rmtree(SITE)
    SITE.mkdir(parents=True)

    shutil.copytree(TEMPLATES, SITE, dirs_exist_ok=True,
                    ignore=shutil.ignore_patterns('overrides'))

    for folder in ('docs/en', 'docs/中文版', 'pitfalls/en', 'pitfalls/中文版'):
        target = SITE / folder
        target.mkdir(parents=True, exist_ok=True)
        for path in sorted((REPO / folder).glob('*.md')):
            shutil.copy2(path, target / path.name)

    write(SITE / 'docs' / 'en' / 'index.md', article_index(
        'docs/en', 'Concepts', 'Every English concept article, one per idea.',
        'en', order=NAV_DISPLAY_ORDER))
    write(SITE / 'docs' / '中文版' / 'index.md', article_index(
        'docs/中文版', '概念文档', '每篇只讲一个想法的中文概念文章。',
        'zh', order=NAV_DISPLAY_ORDER))
    write(SITE / 'pitfalls' / 'en' / 'index.md', article_index(
        'pitfalls/en', 'Pitfalls', 'Every English failure mode, with the fix it implies.',
        'en', PITFALL_GROUPS))
    write(SITE / 'pitfalls' / '中文版' / 'index.md', article_index(
        'pitfalls/中文版', '陷阱文章', '每种失败模式，以及它指向的修复方式。',
        'zh', PITFALL_GROUPS))

    i18n = SITE / 'docs' / 'i18n'
    i18n.mkdir(parents=True, exist_ok=True)
    shutil.copy2(REPO / 'docs' / 'i18n' / 'terminology.md',
                 i18n / 'terminology.md')

    examples = SITE / 'examples'
    examples.mkdir(parents=True, exist_ok=True)
    for path in sorted((REPO / 'examples').glob('*.cpp')):
        shutil.copy2(path, examples / path.name)

    images = SITE / 'assets' / 'images'
    images.mkdir(parents=True, exist_ok=True)
    for name in ('cppuccino-banner-light.svg', 'cppuccino-banner-dark.svg',
                 'value-categories.png', 'cppuccino-social.png'):
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

    validate_template_tokens(tokens)

    normalize_directory_links()
    ensure_directory_indexes()
    add_descriptions(
        'One C++ idea, in two languages, with runnable proof.')

    validate_directory_links()
    validate_inline_labs()
    validate_html_assets()

    # Game data for the Play mode: real content, no duplicated articles.
    write(SITE / 'assets' / 'quests.json',
          json.dumps(build_quests(), indent=2, ensure_ascii=False))

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
