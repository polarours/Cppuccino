#!/usr/bin/env python3
"""Find user callbacks that are called while a lock is held.

A class that takes a callback and calls it while it still holds its own
mutex deadlocks as soon as the callback calls back into the class. The
mutex is not recursive, so the thread waits for itself.

Three projects in this repository had it: the chat server called the
message callback under its lock, the observer debouncer called its
handler under its lock, and both of them hung for ever when the callback
asked for one more event.

The scan walks the blocks that surround a call and asks, for each of
them, whether it took a lock that is still in scope.
"""
import glob
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')
ALLOWLIST = os.path.join(REPO, 'tools', 'known_callback_locks.tsv')

# A name that stands for a callback, so that a member can be declared
# with the alias instead of the type itself.
ALIAS_RE = re.compile(r'using\s+(\w+)\s*=\s*std::function<[^;]*>;')

# A member that holds a callback, written with the type itself.
MEMBER_RE = re.compile(r'std::function<[^;{]*?>\s+(\w+)\s*;')

# A lock that is taken.
LOCK_RE = re.compile(r'(?:lock_guard|unique_lock)\s*<[^>]*>\s*\w+\s*\(')

# A function definition, with its parameters.
DEFINITION_RE = re.compile(
    r'(?:^|\n)[ \t]*(?:[A-Za-z_][\w:<>~\s*&]*?)\b(\w+)\s*\(([^;{}]*?)\)\s*'
    r'(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?(?:->[^;{]*)?\{',
    re.M)


def split_parameters(text):
    """The parameters of a function, one by one.

    The commas inside a template argument list or inside parentheses do
    not separate them.
    """
    parts = []
    depth = 0
    current = ''
    for char in text:
        if char in '<([':
            depth += 1
        elif char in '>)]':
            depth -= 1
        if char == ',' and depth == 0:
            parts.append(current)
            current = ''
        else:
            current += char
    if current.strip():
        parts.append(current)
    return [p.strip() for p in parts if p.strip()]


def parameter_name(text):
    """The name of a parameter and the type it has."""
    text = text.split('=')[0].strip()
    match = re.match(r'([\w:]+(?:\s*<[^>]*>)?(?:\s*\*|&)?)\s+(\w+)$', text)
    if not match:
        return None, None
    return match.group(1).strip(), match.group(2)


def block_end(text, start):
    """The position of the brace that closes the block opening at start."""
    depth = 0
    index = start
    while index < len(text):
        char = text[index]
        if char == '{':
            depth += 1
        elif char == '}':
            depth -= 1
            if depth == 0:
                return index
        index += 1
    return len(text)


def enclosing_blocks(text, position):
    """The blocks that contain position, innermost first."""
    blocks = []
    depth = 0
    index = position
    while index >= 0:
        char = text[index]
        if char == '}':
            depth += 1
        elif char == '{':
            if depth == 0:
                end = block_end(text, index)
                blocks.append((index, end, text[index + 1:end]))
                position = index
            else:
                depth -= 1
        index -= 1
    return blocks


def own_level(text):
    """The text of a block without the blocks nested inside it.

    The nested parts are replaced by spaces, so that a position in the
    result is the same position in the text it came from.
    """
    out = []
    depth = 0
    for char in text:
        if char == '{':
            depth += 1
            out.append(' ')
        elif char == '}':
            depth -= 1
            out.append(' ')
        elif depth == 0:
            out.append(char)
        else:
            out.append(' ')
    return ''.join(out)


def projects():
    for dirpath, dirnames, filenames in os.walk(ROADMAP):
        dirnames[:] = [d for d in dirnames if d != 'build']
        if 'CMakeLists.txt' in filenames:
            yield dirpath


def files_of(project):
    found = []
    for pattern in ('include/*.hpp', 'include/*.h', 'src/*.cpp', 'src/*.hpp',
                    'tests/*.cpp'):
        found.extend(sorted(glob.glob(os.path.join(project, pattern))))
    return found


def allowed():
    """The callbacks that are called under the lock on purpose."""
    entries = set()
    if not os.path.isfile(ALLOWLIST):
        return entries
    with open(ALLOWLIST, encoding='utf-8') as handle:
        for number, line in enumerate(handle, 1):
            if line.startswith('#') or not line.strip():
                continue
            parts = line.rstrip('\n').split('\t')
            if len(parts) < 2:
                print(f'  the allowlist line {number} is not a path and a '
                      f'name')
                continue
            entries.add((parts[0], parts[1]))
    return entries


def main():
    problems = []
    checked = 0
    excused = allowed()
    used = set()

    for project in sorted(projects()):
        paths = files_of(project)
        texts = {}
        for path in paths:
            with open(path, encoding='utf-8', errors='replace') as handle:
                texts[path] = handle.read()

        # The callback members of the whole project: a header declares
        # them and a source file calls them.
        members = set()
        aliases = set()
        for text in texts.values():
            members |= set(MEMBER_RE.findall(text))
            aliases |= set(ALIAS_RE.findall(text))
        if aliases:
            alias_re = re.compile(
                r'(?:' + '|'.join(re.escape(a) for a in sorted(aliases)) +
                r')\s+(\w+)\s*;')
            for text in texts.values():
                members |= set(alias_re.findall(text))
        # A callback can also arrive as a parameter, and a condition
        # variable calls the predicate of its wait with the lock held.
        # The guarded suspension of this repository had one: the predicate
        # of waitFor() asked the object about itself and waited for the
        # lock the thread already had.
        #
        # So the parameters are collected before the project is counted:
        # a project whose only callback is a parameter has no member
        # callback at all.
        callbacks = set(members)
        for path, text in texts.items():
            for definition in DEFINITION_RE.finditer(text):
                body_start = text.find('{', definition.end() - 1)
                if body_start == -1:
                    continue
                depth, index = 1, body_start + 1
                while index < len(text) and depth > 0:
                    if text[index] == '{':
                        depth += 1
                    elif text[index] == '}':
                        depth -= 1
                    index += 1
                body = text[body_start:index]
                for parameter in split_parameters(definition.group(2)):
                    kind, name = parameter_name(parameter)
                    if kind is None:
                        continue
                    if kind in callbacks or kind.split('::')[-1] in aliases:
                        callbacks.add(name)

        if not callbacks:
            continue
        checked += 1

        for path, text in texts.items():
            for member in sorted(callbacks):
                for match in re.finditer(r'\b' + re.escape(member) + r'\s*\(',
                                        text):
                    for start, end, body in enclosing_blocks(text,
                                                             match.start()):
                        flat = own_level(body)
                        locks = list(LOCK_RE.finditer(flat))
                        if not locks:
                            continue
                        call_at = match.start() - (start + 1)
                        if locks[-1].start() < call_at:
                            line = text[:match.start()].count('\n') + 1
                            where = os.path.relpath(path, REPO)
                            if (where, member) in excused:
                                used.add((where, member))
                                break
                            problems.append((where, line, member))
                        break

    # An entry that no longer matches anything is a leftover: the callback
    # it excuses is called with the lock released now, and the reason it
    # was there no longer applies.
    stale = sorted(excused - used)

    print('== callbacks under a lock ==')
    print(f'  {checked} project(s) with a callback')
    print(f'  {len(excused)} callback(s) excused on purpose')
    if stale:
        print(f'\n{len(stale)} excused callback(s) that are not under a lock '
              f'any more:')
        for where, member in stale:
            print(f'  {where}  {member}()')
    if problems or stale:
        print(f'\n{len(problems)} callback(s) called while a lock is held:')
        for path, line, member in problems:
            print(f'  {path}:{line}  {member}()')
        return 1
    if not stale:
        print('\nno callback is called while a lock is held')
    return 1 if stale else 0


if __name__ == '__main__':
    sys.exit(main())
