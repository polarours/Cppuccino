#!/usr/bin/env python3
"""Compile every example under examples/.

Each example states how to build itself in its header comment, for example
"// Compile with: g++ -std=c++20 -o demo demo.cpp". This script reads that
declaration and uses it, so a header that lies about its own requirements is
reported as well. Examples without a declaration are compiled with the first
standard that works.

The examples are compiled and linked but not run: several of them expect
input or start a server.

Usage: python3 tools/check_examples.py [--jobs N]
"""
import argparse
import os
import os
import re
import shutil
import subprocess
import sys
import tempfile
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXAMPLES = os.path.join(REPO, 'examples')

# Examples that build on a roadmap project and need its headers and sources.
PROJECT_DEPENDENT = {
    'task-scheduler-error-handling.cpp': (
        'cpp-mastery-roadmap/05-stage5/task-scheduler',
        ['src/task_scheduler.cpp'],
    ),
    'web-server-benchmark.cpp': (
        'cpp-mastery-roadmap/04-stage4/web-server',
        ['src/web_server.cpp'],
    ),
    'connection-pool-concurrency.cpp': (
        'cpp-mastery-roadmap/01-beginner/student-management/v1',
        [],
    ),
}

STANDARD_RE = re.compile(r'-std=c\+\+(\d+)')
DECL_RE = re.compile(r'^\s*//.*\bcompile\b', re.IGNORECASE)

# Standards tried, in order, for an example that does not declare one.
FALLBACK_STANDARDS = ['20', '23', '26']

# The compiler to use. CXX follows the usual convention, so a CI job can
# point at a newer toolchain without changing this script.
CXX = os.environ.get('CXX', 'g++')

# The flags for the sanitizer pass. An example that reads freed memory or
# does something the standard leaves undefined still runs, and says so on
# the error stream.
SANITIZER_FLAGS = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                   '-g']

# The flags for the thread sanitizer pass. It looks for two threads
# touching the same memory without a lock.
TSAN_FLAGS = ['-fsanitize=thread', '-fno-omit-frame-pointer', '-g', '-O1']

# What the sanitizer writes when it finds something.
SANITIZER_MARKERS = ('AddressSanitizer:', 'LeakSanitizer:',
                     'UndefinedBehaviorSanitizer:', 'runtime error:',
                     'ThreadSanitizer:')


def sanitizer_report(stderr):
    """The first line the sanitizer wrote, or an empty string."""
    for line in stderr.splitlines():
        for marker in SANITIZER_MARKERS:
            if marker in line:
                return line.strip()[:120]
    return ''


def declared_standard(path):
    """The -std= flag the header declares, or None when it declares none."""
    with open(path, encoding='utf-8', errors='replace') as handle:
        for _ in range(40):          # the declaration lives in the header
            line = handle.readline()
            if not line:
                break
            if DECL_RE.match(line):
                match = STANDARD_RE.search(line)
                if match:
                    return match.group(1)
    return None


def compile_commands(path, standard, workdir):
    """The g++ command line for one example."""
    name = os.path.basename(path)
    binary = os.path.join(workdir, name[:-4])
    cmd = [CXX, f'-std=c++{standard}', '-o', binary, path]

    project = PROJECT_DEPENDENT.get(name)
    if project:
        directory, sources = project
        cmd.insert(2, f'-I{os.path.join(REPO, directory)}/include')
        for source in sources:
            cmd.append(os.path.join(REPO, directory, source))
    return cmd


# The compiler rejecting the -std= flag means the toolchain is too old for
# the example, which is not a defect in the example.
TOOLCHAIN_TOO_OLD = ('unrecognized command-line option',
                      'invalid value', 'unknown argument')


def too_old(stderr):
    return any(marker in stderr for marker in TOOLCHAIN_TOO_OLD)


def check_one(path, workdir):
    """Return (status, detail). status is 'ok', 'skip' or 'fail'."""
    name = os.path.basename(path)
    standard = declared_standard(path)

    if standard:
        cmd = compile_commands(path, standard, workdir)
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode == 0:
            return 'ok', f'ok (-std=c++{standard})'
        if too_old(result.stderr):
            return 'skip', (f'the toolchain has no -std=c++{standard}, '
                            'so this example was not checked')
        first = next((l for l in result.stderr.splitlines() if ' error:' in l),
                     result.stderr.splitlines()[0] if result.stderr else '?')
        return 'fail', f'failed with the declared -std=c++{standard}: {first.strip()[:110]}'

    errors = []
    for candidate in FALLBACK_STANDARDS:
        cmd = compile_commands(path, candidate, workdir)
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode == 0:
            return 'ok', f'ok (-std=c++{candidate})'
        if too_old(result.stderr):
            continue
        first = next((l for l in result.stderr.splitlines() if ' error:' in l),
                     result.stderr.splitlines()[0] if result.stderr else '?')
        errors.append(f'{candidate}: {first.strip()[:80]}')
    if not errors:
        return 'skip', 'the toolchain supports none of the standards this example needs'
    return 'fail', 'no standard works -- ' + ' | '.join(errors)


def declared_output(path):
    """The binary name the header says the example produces."""
    text = open(path, encoding='utf-8', errors='replace').read()
    m = re.search(r'^//\s*Compile(?:\s+with)?\s*:\s*(g\+\+.*)$', text, re.M)
    if not m:
        return None
    out = re.search(r'-o\s+(\S+)', m.group(1))
    return os.path.basename(out.group(1)) if out else None


def check_header_convention(sources, problems):
    """Every example must carry the header the repository uses.

    The first line names the file, the second says what it demonstrates, and
    the third gives the command to build it. A reader copies that command, so
    it has to say which standard the file needs: several of them need C++20
    or C++23.
    """
    for path in sources:
        name = os.path.basename(path)
        text = open(path, encoding='utf-8', errors='replace').read()
        lines = text.splitlines()
        if not lines or not lines[0].startswith('//'):
            problems.append(f'{name}: no header comment')
            print(f"  NO HEADER  {name}")
            continue
        if lines[0].strip() != f'// examples/{name}':
            problems.append(f'{name}: the header does not name the file')
            print(f"  HEADER     {name}: {lines[0].strip()!r}")
        # The description may wrap over several lines, so the compile command
        # is looked for anywhere in the leading comment block.
        header_lines = []
        for line in lines:
            if not line.startswith('//'):
                break
            header_lines.append(line)
        if not any(re.match(r'^//\s*Compile (with|\+ run)\s*:', l)
                   for l in header_lines):
            problems.append(f'{name}: the header has no compile command')
            print(f"  HEADER     {name}: no compile command")
        if re.search(r'[\u4e00-\u9fff]', '\n'.join(header_lines)):
            problems.append(f'{name}: the header is not in English')
            print(f"  HEADER     {name}: the header is not in English")


def check_english_body(sources, problems):
    """The body of an example has to be English, like its header.

    The example files carry English names and the index that lists them has
    an English half, so the code and the messages it prints are read by an
    English reader. Seven of the files still had Chinese comments or
    Chinese output, and the header check did not look that far.
    """
    for path in sources:
        name = os.path.basename(path)
        text = open(path, encoding='utf-8', errors='replace').read()
        for number, line in enumerate(text.splitlines(), 1):
            if not re.search(r'[\u4e00-\u9fff]', line):
                continue
            problems.append(f'{name}:{number}: the body is not in English')
            print(f"  CHINESE   {name}:{number}: {line.strip()[:60]}")


def check_output_collisions(sources, problems):
    """Two examples must not write to the same output name.

    Compiling both in the same directory would have the second silently
    overwrite the first.
    """
    targets = defaultdict(list)
    for path in sources:
        name = declared_output(path)
        if name:
            targets[name].append(os.path.basename(path))
    for name, files in sorted(targets.items()):
        if len(files) > 1:
            problems.append(
                f'the examples {", ".join(sorted(files))} all write to '
                f'./{name}')
            print(f"  COLLISION  {name} <- {', '.join(sorted(files))}")


def check_index_translation(problems):
    """The Chinese index must describe the examples in Chinese.

    The example files keep their English headers, so the descriptions in the
    Chinese index are the one place the topic of each example is written in
    Chinese. They have to stay that way.
    """
    index = os.path.join(EXAMPLES, 'README.zh-CN.md')
    if not os.path.isfile(index):
        problems.append('the Chinese examples index is missing')
        print('  NO INDEX  examples/README.zh-CN.md')
        return
    for number, line in enumerate(
            open(index, encoding='utf-8').read().splitlines(), 1):
        m = re.match(r'^- \[[^\]]+\]\(([^)]+)\) — (.*)$', line)
        if not m:
            continue
        description = m.group(2)
        if len(re.findall(r'[\u4e00-\u9fff]', description)) < 2:
            problems.append(
                f'the Chinese index does not describe {m.group(1)} in Chinese')
            print(f"  NOT CHINESE  {m.group(1)}: {description[:60]}")


def check_index_coverage(sources, problems):
    """The index must list every example.

    An example nobody can find is an example nobody reads.
    """
    index = os.path.join(EXAMPLES, 'README.md')
    if not os.path.isfile(index):
        problems.append('the examples index is missing')
        print('  NO INDEX  examples/README.md')
        return
    listed = set(re.findall(r'^- \[([^\]]+\.cpp)\]',
                             open(index, encoding='utf-8').read(), re.M))
    for path in sources:
        name = os.path.basename(path)
        if name not in listed:
            problems.append(f'the examples index does not list {name}')
            print(f"  UNLISTED  {name}")
    print(f"  {len(listed)} example(s) listed in the index")


def run_example(path, standard, workdir, sanitizer=None):
    """Compile and run an example, from a directory that is not the repo.

    Running from elsewhere is the point: an example that assumes the current
    directory is the repository root crashes for a reader who just followed
    the command in its header.
    """
    name = os.path.basename(path)
    binary = os.path.join(workdir, 'run_' + name[:-4])
    cmd = [CXX, f'-std=c++{standard}', '-pthread', '-o', binary, path]
    if sanitizer:
        cmd += sanitizer
    project = PROJECT_DEPENDENT.get(name)
    if project:
        directory, sources = project
        cmd.insert(2, f'-I{os.path.join(REPO, directory)}/include')
        for source in sources:
            cmd.append(os.path.join(REPO, directory, source))
    built = subprocess.run(cmd, capture_output=True, text=True,
                           errors='replace')
    if built.returncode != 0:
        return 'compile', 'the example does not compile'
    try:
        result = subprocess.run([binary], cwd=workdir, capture_output=True,
                                text=True, errors='replace', timeout=20)
    except subprocess.TimeoutExpired:
        return 'run', 'the example did not finish'
    if sanitizer:
        report = sanitizer_report(result.stderr or '')
        if report:
            return 'sanitize', report
    if result.returncode != 0:
        detail = (result.stderr.strip().splitlines() or ['?'])[-1]
        return 'run', f'the example exited with {result.returncode}: {detail[:70]}'
    return 'ok', ''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', '-j', type=int, default=os.cpu_count() or 2)
    parser.add_argument('--run', action='store_true',
                        help='also run every example and report the ones that '
                             'do not finish cleanly')
    parser.add_argument('--sanitize', action='store_true',
                        help='run every example under the sanitizers and '
                             'report the ones that read freed memory or do '
                             'something the standard leaves undefined')
    parser.add_argument('--tsan', action='store_true',
                        help='run every example under the thread sanitizer '
                             'and report the ones with a data race')
    args = parser.parse_args()

    if shutil.which(CXX) is None:
        print(f'the compiler {CXX!r} was not found')
        return 1

    sources = sorted(os.path.join(EXAMPLES, name)
                     for name in os.listdir(EXAMPLES) if name.endswith('.cpp'))
    if not sources:
        print('no examples found')
        return 1

    workdir = tempfile.mkdtemp(prefix='cppuccino-examples-')
    failures = []
    collisions = []
    check_output_collisions(sources, collisions)
    check_header_convention(sources, collisions)
    check_english_body(sources, collisions)
    check_index_coverage(sources, collisions)
    check_index_translation(collisions)

    try:
        with ThreadPoolExecutor(max_workers=args.jobs) as pool:
            results = list(pool.map(
                lambda p: (p, check_one(p, workdir)), sources))

        for path, (status, detail) in results:
            name = os.path.basename(path)
            if status == 'ok':
                print(f'  {name:<48} {detail}')
            elif status == 'skip':
                print(f'  {name:<48} SKIP {detail}')
            else:
                print(f'  {name:<48} FAIL {detail}')
                failures.append((name, detail))

        passes = []
        if args.run:
            passes.append(('run check', None))
        if args.sanitize:
            passes.append(('sanitizer check', SANITIZER_FLAGS))
        if args.tsan:
            passes.append(('thread sanitizer check', TSAN_FLAGS))
        for title, flags in passes:
            print(f'\n== {title} ==')
            with ThreadPoolExecutor(max_workers=args.jobs) as pool:
                runs = list(pool.map(
                    lambda p: (p, run_example(
                        p, declared_standard(p) or '20', workdir,
                        sanitizer=flags)),
                    sources))
            for path, (status, detail) in runs:
                if status != 'ok':
                    failures.append((os.path.basename(path), detail))
                    print(f"  {os.path.basename(path):<44} "
                          f"{status.upper()} {detail}")
    finally:
        shutil.rmtree(workdir, ignore_errors=True)

    print()
    print(f'{len(sources) - len(failures)}/{len(sources)} examples compiled')
    if failures:
        print(f'\n{len(failures)} failure(s):')
        for name, detail in failures:
            print(f'  {name}: {detail}')
    if collisions:
        print(f'\n{len(collisions)} output name collision(s)')
        for c in collisions:
            print(f'  {c}')
    return 1 if (failures or collisions) else 0


if __name__ == '__main__':
    sys.exit(main())
