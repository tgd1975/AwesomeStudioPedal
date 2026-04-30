#!/usr/bin/env bash
# scripts/commit-pathspec.sh — wrapper for git's pathspec-form commit
#
# This is the executable behind the /commit skill. It writes the
# provenance token at .git/asp-commit-token, runs `git add` for any
# untracked files in the pathspec, then runs
# `git commit -m "..." -- <files>`.
#
# The pre-commit hook validates the token and rejects commits that
# did not come from this wrapper. See docs/developers/COMMIT_POLICY.md
# for the full design.
#
# Usage:
#   scripts/commit-pathspec.sh "<message>" <file> [<file> …]
#   scripts/commit-pathspec.sh --no-verify "<message>" <file> [<file> …]
#
# Exit codes:
#   0  commit succeeded
#   1  commit failed (hook, git, or invalid args)

set -euo pipefail

REPO_ROOT="$(git rev-parse --show-toplevel)"
GIT_DIR="$(git rev-parse --git-dir)"

TOKEN_FILE="${GIT_DIR}/asp-commit-token"

# Parse --no-verify flag (must come before message if present).
NO_VERIFY=""
if [ "${1:-}" = "--no-verify" ]; then
    NO_VERIFY="--no-verify"
    shift
fi

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 [--no-verify] \"<message>\" <file> [<file> …]" >&2
    exit 1
fi

MESSAGE="$1"
shift

# Reject empty pathspec — the whole point is explicit file lists.
if [ "$#" -eq 0 ]; then
    echo "ERROR: no files in pathspec. /commit requires at least one file." >&2
    exit 1
fi

# Step 1: git add only the untracked files in the pathspec.
# Tracked files (modified or unmodified) go in via the pathspec on the
# git-commit invocation; staging them now would defeat the parallel-
# session safety property.
for f in "$@"; do
    if ! git ls-files --error-unmatch -- "$f" >/dev/null 2>&1; then
        # Untracked — needs to be added so pathspec can see it.
        git add -- "$f"
    fi
done

# Step 2: write the provenance token. Format: <pid> <nonce> <unix-ts>.
# PID is the shell that will run `git commit` next (i.e. $$).
PID="$$"
NONCE="$(head -c 16 /dev/urandom | od -An -tx1 | tr -d ' \n')"
NOW="$(date +%s)"
echo "${PID} ${NONCE} ${NOW}" > "${TOKEN_FILE}"

# Best-effort cleanup if anything below this point fails before the
# hook reads (and deletes) the token. Without this, a stale token
# could authorise a later raw `git commit` within the 60s TTL.
trap 'rm -f "${TOKEN_FILE}"' EXIT

# Step 3: commit in pathspec form. The hook will validate and delete
# the token. On hook success, EXIT trap runs but file is already gone
# (rm -f is no-op).
if [ -n "${NO_VERIFY}" ]; then
    git commit "${NO_VERIFY}" -m "${MESSAGE}" -- "$@"
else
    git commit -m "${MESSAGE}" -- "$@"
fi
