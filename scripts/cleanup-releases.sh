#!/usr/bin/env bash
# cleanup-releases.sh — keep only the current + 2 previous GitHub releases.
#
# Deletes older releases and their tags using the GitHub CLI (gh).
# Safe to run multiple times (idempotent).
#
# Usage:
#   bash scripts/cleanup-releases.sh [--dry-run]
#
# Options:
#   --dry-run   Print what would be deleted without actually deleting anything.
#
# Requirements:
#   - gh CLI authenticated (gh auth login)
#   - Write permission on the repository
#
# See the Release Process section in CONTRIBUTING.md for context.

set -euo pipefail

DRY_RUN=false
if [[ "${1:-}" == "--dry-run" ]]; then
    DRY_RUN=true
    echo "DRY RUN — no releases will be deleted"
fi

KEEP=3  # current + 2 previous

# Fetch all vX.Y.Z tags in version order (oldest first)
tags=$(git tag -l 'v*' | sort -V)
total=$(echo "$tags" | grep -c . || true)

if [[ "$total" -le "$KEEP" ]]; then
    echo "Only $total release(s) found — nothing to clean up (keeping $KEEP)."
    exit 0
fi

keep_tags=$(echo "$tags" | tail -n "$KEEP")
delete_tags=$(echo "$tags" | head -n $(( total - KEEP )))

echo "Keeping:  $(echo "$keep_tags" | tr '\n' ' ')"
echo "Deleting: $(echo "$delete_tags" | tr '\n' ' ')"

for tag in $delete_tags; do
    if $DRY_RUN; then
        echo "[dry-run] would delete release and tag: $tag"
    else
        echo "Deleting release and tag: $tag"
        gh release delete "$tag" --yes --cleanup-tag
    fi
done

echo "Done."
