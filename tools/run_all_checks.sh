#!/usr/bin/env bash
# Run every repository check, in the same order the CI does.
#
# Usage:
#   tools/run_all_checks.sh            # documentation checks only
#   tools/run_all_checks.sh --full     # also build and test every project
#
# The documentation checks are fast enough to run before every commit. The
# full run builds and tests all projects, which takes a while.

set -uo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

FULL=0
if [ "${1:-}" = "--full" ]; then
    FULL=1
fi

FAILED=()

run() {
    local name="$1"
    shift
    echo ""
    echo "=============================================================="
    echo "== $name"
    echo "=============================================================="
    if "$@"; then
        echo "-- $name: OK"
    else
        echo "-- $name: FAILED"
        FAILED+=("$name")
    fi
}

run "documentation: language and bilingual parity" \
    python3 tools/check_docs.py

run "documentation: relative links" \
    python3 tools/check_links.py

run "documentation: examples compile and run" \
    python3 tools/check_doc_examples.py

if [ "$FULL" = "1" ]; then
    run "projects: configure, build and test" \
        python3 tools/build_all_projects.py
fi

echo ""
echo "=============================================================="
if [ ${#FAILED[@]} -eq 0 ]; then
    echo "all checks passed"
    exit 0
fi
echo "${#FAILED[@]} check(s) failed:"
for name in "${FAILED[@]}"; do
    echo "  - $name"
done
exit 1
