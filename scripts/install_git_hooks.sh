#!/usr/bin/env bash
# Install repo-side git hooks into .git/hooks/.
# Symlinks scripts/git-hooks/* into .git/hooks/* so future hook-script changes
# are picked up automatically. Falls back to copy if symlinks fail (e.g. some
# Windows + Git Bash setups).
#
# Re-run is idempotent. Existing non-link / non-matching files are backed up
# to <name>.backup.<timestamp> rather than overwritten.
set -euo pipefail

REPO_ROOT="$(git rev-parse --show-toplevel)"
SRC_DIR="$REPO_ROOT/scripts/git-hooks"
DST_DIR="$REPO_ROOT/.git/hooks"

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

if [ ! -d "$SRC_DIR" ]; then
    echo -e "${RED}error: $SRC_DIR not found${NC}" >&2
    exit 1
fi
if [ ! -d "$DST_DIR" ]; then
    mkdir -p "$DST_DIR"
fi

ts() { date +%Y%m%d-%H%M%S; }

install_one() {
    local name="$1"
    local src="$SRC_DIR/$name"
    local dst="$DST_DIR/$name"

    chmod +x "$src" 2>/dev/null || true

    if [ -L "$dst" ]; then
        local cur
        cur="$(readlink "$dst" || true)"
        if [ "$cur" = "$src" ] || [ "$cur" = "../../scripts/git-hooks/$name" ]; then
            echo -e "  ${GREEN}ok${NC}    $name (already linked)"
            return
        fi
        echo -e "  ${YELLOW}backup${NC} $name -> $name.backup.$(ts) (was symlink to $cur)"
        mv "$dst" "$dst.backup.$(ts)"
    elif [ -e "$dst" ]; then
        echo -e "  ${YELLOW}backup${NC} $name -> $name.backup.$(ts)"
        mv "$dst" "$dst.backup.$(ts)"
    fi

    if ln -s "../../scripts/git-hooks/$name" "$dst" 2>/dev/null; then
        echo -e "  ${GREEN}link${NC}  $name"
    else
        cp "$src" "$dst"
        chmod +x "$dst"
        echo -e "  ${YELLOW}copy${NC}  $name (symlink failed; re-run installer after hook script changes)"
    fi
}

echo "Installing security-review git hooks into .git/hooks/:"
for h in pre-merge-commit post-merge pre-rebase; do
    install_one "$h"
done

echo ""
echo -e "${GREEN}done.${NC} Hooks active. To bypass: ASP_SKIP_SECURITY_REVIEW=1 git pull"
echo "Reports are written to .claude/security-review-latest.md"
