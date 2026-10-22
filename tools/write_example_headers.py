#!/usr/bin/env python3
"""Give every example the header the repository uses.

The convention, followed by 88 of the 120 examples, is:

    // examples/<filename>.cpp
    // <one line saying what it demonstrates>
    // Compile with: g++ -std=c++NN -o <name> <filename>.cpp

Thirty-two examples did not follow it: twenty-seven had no header at all, four
had a Chinese one, and one was missing its compile line. A reader who copies
the command from the header needs to know which standard the file needs, and
several of them need C++20 or C++23, so the missing line was not a small
gap.

Usage: python3 tools/write_example_headers.py
"""
import os
import re
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXAMPLES = os.path.join(REPO, 'examples')

# The standard each example needs, and what it demonstrates.
HEADERS = {
    'cpp20-concepts-complete.cpp': (
        '20', 'Demonstrates C++20 concepts, requires clauses and constrained templates.'),
    'crtp-basics.cpp': (
        '17', 'Demonstrates the Curiously Recurring Template Pattern for static polymorphism.'),
    'fold-expressions.cpp': (
        '17', 'Demonstrates C++17 fold expressions for summing, multiplying and concatenating.'),
    'move-semantics-example.cpp': (
        '17', 'Demonstrates move semantics and how copying differs from moving.'),
    'sfinae-type-traits.cpp': (
        '17', 'Demonstrates SFINAE-based type traits and custom detection idioms.'),
    'smart-pointer-shared.cpp': (
        '17', 'Demonstrates std::shared_ptr for shared ownership.'),
    'smart-pointer-unique.cpp': (
        '17', 'Demonstrates std::unique_ptr for exclusive ownership.'),
    'std-apply-invoke.cpp': (
        '17', 'Demonstrates std::apply and std::invoke for calling generic callables.'),
    'std-bit-cpp20.cpp': (
        '20', 'Demonstrates the C++20 <bit> library: popcount, bit_width and bit utilities.'),
    'std-chrono-advanced.cpp': (
        '17', 'Demonstrates advanced std::chrono: durations, clocks and time points.'),
    'std-chrono-format.cpp': (
        '20', 'Demonstrates formatting std::chrono durations and time points with std::format.'),
    'std-chrono-literals.cpp': (
        '20', 'Demonstrates the C++20 chrono literals, such as 100ms and 2s.'),
    'std-clamp.cpp': (
        '17', 'Demonstrates std::clamp for restricting a value to a range.'),
    'std-expected.cpp': (
        '23', 'Demonstrates std::expected for returning either a value or an error.'),
    'std-filesystem-advanced.cpp': (
        '17', 'Demonstrates advanced std::filesystem: recursion, permissions and symlinks.'),
    'std-filesystem-cpp20.cpp': (
        '17', 'Demonstrates std::filesystem for paths, directories and iteration.'),
    'std-flat-map.cpp': (
        '23', 'Demonstrates the C++23 std::flat_map and std::flat_set.'),
    'std-format.cpp': (
        '20', 'Demonstrates std::format string formatting and format specifications.'),
    'std-jthread.cpp': (
        '20', 'Demonstrates std::jthread, with automatic joining and stop tokens.'),
    'std-latch-barrier.cpp': (
        '20', 'Demonstrates std::latch and std::barrier for coordinating threads.'),
    'std-mdspan.cpp': (
        '23', 'Demonstrates the C++23 std::mdspan multidimensional view.'),
    'std-optional-advanced.cpp': (
        '17', 'Demonstrates advanced std::optional use: chaining, value_or and monadic calls.'),
    'std-print-cpp23.cpp': (
        '20', 'Demonstrates the C++23 std::print and std::println output functions.'),
    'std-ranges-advanced.cpp': (
        '20', 'Demonstrates advanced std::ranges: views, adaptors and algorithms.'),
    'std-source-location.cpp': (
        '20', 'Demonstrates std::source_location for compile-time source information.'),
    'std-string-view-advanced.cpp': (
        '17', 'Demonstrates advanced std::string_view: trimming, splitting and lifetime rules.'),
    'std-variant-advanced.cpp': (
        '17', 'Demonstrates advanced std::variant use: visitation, get_if and monadic calls.'),
    # These four had a Chinese header instead of the convention.
    'cpp20-coroutines-complete.cpp': (
        '20', 'Demonstrates C++20 coroutines: generators and tasks.'),
    'cpp23-features-complete.cpp': (
        '23', 'Demonstrates the C++23 features used across this repository.'),
    'std-print-complete.cpp': (
        '23', 'Demonstrates the C++23 std::print and std::println output functions.'),
    'std-span-complete.cpp': (
        '20', 'Demonstrates std::span: views over contiguous sequences.'),
    # This one had the first line but no compile line.
    'const-vs-non-const.cpp': (
        '17', 'Demonstrates the performance difference between const and non-const methods.'),
}


def output_name(path):
    """The binary name the header should use."""
    stem = os.path.basename(path)[:-4]
    return stem.replace('-', '_')


def main():
    written = 0
    for name, (standard, description) in sorted(HEADERS.items()):
        path = os.path.join(EXAMPLES, name)
        if not os.path.isfile(path):
            print(f'WARNING: {name} does not exist')
            continue
        text = open(path, encoding='utf-8').read()

        # Drop whatever header the file already has.
        lines = text.splitlines(keepends=True)
        index = 0
        while index < len(lines) and lines[index].startswith('//'):
            index += 1
        body = ''.join(lines[index:])

        header = (f'// examples/{name}\n'
                  f'// {description}\n'
                  f'// Compile with: g++ -std=c++{standard} '
                  f'-o {output_name(name)} {name}\n\n')
        open(path, 'w', encoding='utf-8').write(header + body)
        written += 1
    print(f'wrote a header for {written} example(s)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
