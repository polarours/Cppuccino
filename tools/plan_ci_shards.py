#!/usr/bin/env python3
"""Plan how to split the roadmap stages over CI shards.

The stages are very uneven: 02-intermediate holds seven projects while most
stages hold three or four. A plain round-robin would leave one shard doing
much more work than the others, so the stages are sorted by size and handed
out to the currently lightest shard.

Usage:
    python3 tools/plan_ci_shards.py [--shards N] [--json]
"""
import argparse
import json
import os
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROADMAP = os.path.join(REPO, 'cpp-mastery-roadmap')


def stage_projects():
    """Every stage directory with the number of projects it holds."""
    stages = []
    for name in sorted(os.listdir(ROADMAP)):
        path = os.path.join(ROADMAP, name)
        if not os.path.isdir(path) or not name[0].isdigit():
            continue
        count = 0
        for dirpath, dirnames, filenames in os.walk(path):
            dirnames[:] = [d for d in dirnames if d != 'build']
            if 'CMakeLists.txt' in filenames:
                count += 1
        if count:
            stages.append((name, count))
    return stages


def plan(stages, shard_count):
    """Hand the stages to the shards, keeping them evenly loaded."""
    buckets = [[] for _ in range(shard_count)]
    loads = [0] * shard_count
    for name, count in sorted(stages, key=lambda s: -s[1]):
        lightest = min(range(shard_count), key=lambda i: loads[i])
        buckets[lightest].append(name)
        loads[lightest] += count
    # Keep the stage order inside a shard stable and readable.
    for bucket in buckets:
        bucket.sort()
    return buckets


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--shards', type=int, default=10)
    parser.add_argument('--json', action='store_true',
                        help='print the shard plan as JSON')
    args = parser.parse_args()

    stages = stage_projects()
    buckets = plan(stages, args.shards)

    if args.json:
        payload = [
            {'id': index + 1, 'stages': bucket}
            for index, bucket in enumerate(buckets) if bucket
        ]
        print(json.dumps(payload))
        return 0

    total = sum(count for _, count in stages)
    for index, bucket in enumerate(buckets):
        if not bucket:
            continue
        load = sum(c for n, c in stages if n in bucket)
        print(f'shard {index + 1}: {load:>2} projects  {", ".join(bucket)}')
    print(f'\n{total} projects over {len(stages)} stages, '
          f'{sum(1 for b in buckets if b)} shards')
    return 0


if __name__ == '__main__':
    sys.exit(main())
