#!/usr/bin/env python3
"""Point each document at the runnable example that demonstrates it.

Thirty-three of the demo examples were not linked from any document, so a
reader of, say, the decorator article had no way to find the decorator demo
without already knowing the file name.

This inserts a short "Example" section, just before the summary, that links
the example. Both language halves get the section, so the two stay in step.

Usage: python3 tools/link_docs_to_examples.py [--dry-run]
"""
import argparse
import glob
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCS_EN = os.path.join(REPO, 'docs', 'en')
DOCS_ZH = os.path.join(REPO, 'docs', '中文版')
EXAMPLES = os.path.join(REPO, 'examples')


def example_stem(path):
    return os.path.basename(path)[:-4]


def describe(path):
    text = open(path, encoding='utf-8', errors='replace').read()
    out = []
    for line in text.splitlines()[1:]:
        if not line.startswith('//'):
            break
        if re.match(r'^//\s*Compile', line):
            break
        out.append(line[2:].strip())
    return ' '.join(out)


def normalise(name):
    return (name.replace('-pattern', '').replace('-demo', '')
                .replace('demo-', ''))


def pair_docs():
    """Match each English document with its Chinese twin.

    The two sets share no filename convention, so the pairing uses the same
    rule as tools/check_docs.py: the ordered list of tokenised code blocks.
    """
    import importlib.util
    spec = importlib.util.spec_from_file_location(
        'check_docs', os.path.join(REPO, 'tools', 'check_docs.py'))
    check_docs = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(check_docs)

    zh_by_sig = {}
    for path in glob.glob(os.path.join(DOCS_ZH, '*.md')):
        sig = check_docs.code_signature(path)
        if sig is not None:
            zh_by_sig.setdefault(sig, []).append(path)

    pairs = {}
    for path in glob.glob(os.path.join(DOCS_EN, '*.md')):
        sig = check_docs.code_signature(path)
        cands = zh_by_sig.get(sig, []) if sig is not None else []
        if len(cands) == 1:
            pairs[os.path.basename(path)] = os.path.basename(cands[0])
    return pairs


# Concept documents whose example is not named after the document.
CURATED = {
    'auto-decltype-and-decltype-auto': 'auto-decltype',
    'copy-elision-and-prvalue-materialization': 'copy-elision',
    'coroutines-lifetime-and-suspension-points': 'cpp20-coroutines',
    'cpp20-concepts': 'cpp20-concepts-complete',
    'factory-method-pattern': 'factory-pattern-demo',
    'from-value-categories-to-reference-binding': 'value-categories',
    'interpreter-pattern': 'interpreter-pattern-demo',
    'noexcept-and-move-operations': 'noexcept-move',
    'pimpl-and-abi-stability': 'pimpl-idiom',
    'ranges-pattern': 'std-ranges-advanced',
    'reference-binding-rules': 'std-string-view',
    'returning-reference-to-local-object': 'pitfall-dangling-reference',
    'sfinae-misuse': 'sfinae-type-traits',
    'state-pattern': 'state-machine-demo',
    'template-instantiation-and-two-phase-lookup': 'template-instantiation',
    'the-true-semantics-of-const': 'const-vs-non-const',
    'understanding-forwarding-references-and-perfect-forwarding': 'perfect-forwarding',
    'understanding-the-semantics-of-std::move': 'move-semantics-example',
    'virtual-dispatch-and-object-model': 'virtual-dispatch-example',
    'iterator-pattern': 'pitfall-iterator-invalidation',
    'object-slicing': 'pitfall-object-slicing',
    'ownership-and-lifetime': 'smart-pointers-ownership',
}


def find_example(doc_stem, example_stems):
    """The example that demonstrates this document, if there is one."""
    if doc_stem in example_stems:
        return doc_stem
    if doc_stem in CURATED and CURATED[doc_stem] in example_stems:
        return CURATED[doc_stem]
    normalised = {normalise(s): s for s in example_stems}
    candidate = normalised.get(normalise(doc_stem))
    return candidate


def insert_section(path, heading, body, dry_run):
    """Insert a section just before the final summary section."""
    text = open(path, encoding='utf-8').read()
    if body.strip() in text:
        return False                      # already linked

    lines = text.splitlines(keepends=True)
    # The section goes just before the closing section of the document, so
    # it does not end up after a "See Also" block.
    closing = None
    for index, line in enumerate(lines):
        title = line.rstrip()
        if re.match(r'^##\s+(\d+\.\s*)?(Summary|Conclusion|总结|结论)\s*$', title):
            closing = index
    if closing is None:
        for index, line in enumerate(lines):
            if re.match(r'^##\s+(See Also|另请参阅)\s*$', line.rstrip()):
                closing = index
                break
    if closing is None:
        block = f'\n{heading}\n\n{body}\n'
        new = ''.join(lines).rstrip('\n') + '\n' + block
    else:
        block = f'{heading}\n\n{body}\n\n'
        new = ''.join(lines[:closing]) + block + ''.join(lines[closing:])

    if not dry_run:
        open(path, 'w', encoding='utf-8').write(new)
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()

    example_paths = {example_stem(p): p for p in glob.glob(os.path.join(EXAMPLES, '*.cpp'))}
    pairs = pair_docs()
    changed = 0

    for en_name, zh_name in sorted(pairs.items()):
        en_stem = en_name[:-3]
        demo = find_example(en_stem, example_paths)
        if not demo:
            continue
        if en_stem not in CURATED:
            # Only link a demo when its own description names the same topic.
            description = describe(example_paths[demo])
            key = normalise(en_stem).replace('pattern', '')
            if key and key not in normalise(demo) and key not in description.lower():
                continue

        en_body = (f'A runnable version of this topic:\n\n'
                   f'- [{demo}.cpp](../../examples/{demo}.cpp)')
        zh_body = (f'这个主题的可运行版本：\n\n'
                   f'- [{demo}.cpp](../../examples/{demo}.cpp)')

        if insert_section(os.path.join(DOCS_EN, en_name),
                          '## Example', en_body, args.dry_run):
            changed += 1
            print(f'  {en_name}: added an Example section')
        if insert_section(os.path.join(DOCS_ZH, zh_name),
                          '## 示例', zh_body, args.dry_run):
            changed += 1
            print(f'  {zh_name}: added an Example section')

    print(f'\n{changed} section(s) {"would be " if args.dry_run else ""}added')
    return 0


if __name__ == '__main__':
    sys.exit(main())
