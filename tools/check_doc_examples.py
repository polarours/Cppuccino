#!/usr/bin/env python3
"""Compile and run the complete example of every bilingual doc.

Two families of documents are handled:

* Self-contained pattern docs (docs/en/*.md, docs/中文版/*.md): the runnable
  example is assembled from the code blocks that precede the `int main()`
  block, then compiled and executed.
* Project API docs: the example needs the project's own headers and sources,
  so the project directory is supplied via the API_DOC_PROJECTS table below.

Usage: python3 tools/check_doc_examples.py [doc.md ...]
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# The compiler to use; CXX follows the usual convention.
CXX = os.environ.get('CXX', 'g++')

# Per-process scratch directory: two checker runs must not clobber each
# other's intermediate files.
WORKDIR = tempfile.mkdtemp(prefix='doc-ex-check-')

# API doc -> (project dir, extra source files relative to the project).
# Keyed by the English slug; the Chinese twin is matched by the slug that
# appears inside its filename (e.g. "Chat服务器API文档.md" -> chat-server-api).
API_DOC_PROJECTS = {
    'chat-server-api': ('cpp-mastery-roadmap/04-stage4/chat-server',
                        ['src/chat_server.cpp']),
    'task-scheduler-api': ('cpp-mastery-roadmap/05-stage5/task-scheduler',
                            ['src/task_scheduler.cpp']),
    'simple-db-api': ('cpp-mastery-roadmap/04-stage4/simple-db',
                       ['src/simple_db.cpp']),
    'connection-pool-api': ('cpp-mastery-roadmap/06-stage6/connection-pool',
                             []),
}

# Chinese filename -> English slug, for the API docs above.
API_DOC_ZH_ALIASES = {
    'Chat服务器API文档.md': 'chat-server-api',
    '任务调度器API文档.md': 'task-scheduler-api',
    'simple-db-API文档.md': 'simple-db-api',
    '连接池API文档.md': 'connection-pool-api',
}

# Examples that take a port number as argv[1]; the checker passes a free one
# so the run does not depend on a well-known port being available.
PORT_ARG_DOCS = {
    'chat-server-api',
}

# The C++ standards the checker tries, newest first for the fallback.
STANDARDS = ['-std=c++20', '-std=c++23', '-std=c++26']

# The flags for the sanitizer pass. An example that reads freed memory or
# does something the standard leaves undefined still runs, and says so on
# the error stream.
SANITIZER_FLAGS = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                   '-g']

# What the sanitizer writes when it finds something.
SANITIZER_MARKERS = ('AddressSanitizer:', 'LeakSanitizer:',
                     'UndefinedBehaviorSanitizer:', 'runtime error:')


def sanitizer_report(stderr):
    """The first line the sanitizer wrote, or an empty string."""
    for line in stderr.splitlines():
        for marker in SANITIZER_MARKERS:
            if marker in line:
                return line.strip()[:120]
    return ''


def free_port():
    """Ask the OS for a port that is currently unused."""
    import socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('127.0.0.1', 0))
        return s.getsockname()[1]


def blocks(path):
    text = open(path, encoding='utf-8').read()
    return re.findall(r'```cpp\n(.*?)```', text, re.S)


def assemble(bl, start):
    """Concatenate blocks[start..main_idx] into one program.

    Dropping duplicate #include lines keeps the result compilable when a
    block repeats an include that an earlier block already pulled in.
    """
    main_idx = next((i for i, b in enumerate(bl) if 'int main' in b), None)
    if main_idx is None or start > main_idx:
        return None
    seen = set()
    kept = []
    for p in bl[start:main_idx] + [bl[main_idx]]:
        lines = p.splitlines(keepends=True)
        out = []
        for ln in lines:
            s = ln.strip()
            if s.startswith('#include'):
                if s in seen:
                    continue
                seen.add(s)
            out.append(ln)
        kept.append(''.join(out))
    return ''.join(kept)


def candidate_starts(bl):
    """Block indices that could begin a run of declarations.

    A run starts at a block that carries #include (the implementation
    section) or at the main block itself when the example is self-contained.
    """
    main_idx = next((i for i, b in enumerate(bl) if 'int main' in b), None)
    if main_idx is None:
        return []
    starts = [i for i, b in enumerate(bl[:main_idx]) if '#include' in b]
    starts.append(main_idx)
    return sorted(set(starts), reverse=True)


def api_slug_for(path):
    """Return the English slug of an API doc, or None for other documents."""
    base = os.path.basename(path)
    if base in API_DOC_ZH_ALIASES:
        return API_DOC_ZH_ALIASES[base]
    for key in API_DOC_PROJECTS:
        if base.startswith(key):
            return key
    return None


def api_project_for(path):
    """Map a doc path to its (project dir, sources) if it is a known API doc."""
    slug = api_slug_for(path)
    if slug is None:
        return None
    return API_DOC_PROJECTS[slug]


def check(path, sanitize=False):
    """Return (status, detail). status is 'ok', 'skip' or 'fail'.

    The program is assembled by trying each plausible starting block, from
    the most recent one backwards, and keeping the first that compiles. This
    keeps the run minimal and avoids dragging in an unrelated earlier
    example that happens to declare the same types.
    """
    bl = blocks(path)
    starts = candidate_starts(bl)
    if not starts:
        return 'skip', 'no main() block'

    src = os.path.join(WORKDIR, 'example.cpp')
    exe = os.path.join(WORKDIR, 'example')

    api = api_project_for(path)
    base_cmd = [CXX, '-pthread', '-o', exe]
    if sanitize:
        base_cmd += SANITIZER_FLAGS
    if api:
        proj, sources = api
        base_cmd.append(f'-I{os.path.join(REPO, proj)}/include')
        for s in sources:
            base_cmd.append(os.path.join(REPO, proj, s))

    run_args = []
    api_slug = api_slug_for(path)
    if api_slug in PORT_ARG_DOCS:
        run_args = [str(free_port())]

    last_error = ''
    for start in starts:
        prog = assemble(bl, start)
        if prog is None:
            continue
        open(src, 'w').write(prog)
        for std_flag in STANDARDS:
            c = subprocess.run(base_cmd + [std_flag, src], capture_output=True, text=True)
            if c.returncode == 0:
                try:
                    r = subprocess.run([exe] + run_args, capture_output=True,
                                       text=True, timeout=30)
                except subprocess.TimeoutExpired:
                    return 'fail', 'run: timed out'
                if sanitize:
                    report = sanitizer_report(r.stderr or '')
                    if report:
                        return 'fail', f'sanitizer: {report}'
                if r.returncode != 0:
                    return 'fail', f'run: exit {r.returncode}'
                return 'ok', f'{len(r.stdout.strip().splitlines())} output lines'
            lines = c.stderr.strip().splitlines()
            last_error = next((l for l in lines if ' error:' in l),
                              lines[0] if lines else '?')
    return 'fail', f'compile: {last_error.strip()[:100]}'


def main():
    args = sys.argv[1:]
    sanitize = False
    if '--sanitize' in args:
        sanitize = True
        args.remove('--sanitize')
    paths = args
    if not paths:
        paths = []
        for d in ('docs/en', 'docs/中文版'):
            paths += sorted(os.path.join(REPO, d, f)
                            for f in os.listdir(os.path.join(REPO, d))
                            if f.endswith('.md'))
    failures = 0
    for p in paths:
        status, detail = check(p, sanitize)
        rel = os.path.relpath(p, REPO)
        if status == 'ok':
            print(f"  {rel:<52} OK   {detail}")
        elif status == 'skip':
            print(f"  {rel:<52} SKIP {detail}")
        else:
            failures += 1
            print(f"  {rel:<52} FAIL {detail}")
    print(f"\n{failures} failure(s)")
    return 1 if failures else 0


if __name__ == '__main__':
    try:
        code = main()
    finally:
        shutil.rmtree(WORKDIR, ignore_errors=True)
    sys.exit(code)
