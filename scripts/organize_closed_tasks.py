#!/usr/bin/env python3
"""
Move all .md files from docs/developers/tasks/closed/ into a version subdirectory.

Usage:
  python scripts/organize_closed_tasks.py v0.3.0
  python scripts/organize_closed_tasks.py v0.3.0 --dry-run

The script:
1. Creates docs/developers/tasks/closed/<version>/
2. Moves every .md file from the root closed/ directory into that subdirectory.
3. Updates the OVERVIEW.md via update_task_overview.py.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys

CLOSED_DIR = "docs/developers/tasks/closed"


def validate_version(version: str) -> bool:
    return bool(re.fullmatch(r"v\d+\.\d+\.\d+", version))


def main():
    parser = argparse.ArgumentParser(description="Organize closed tasks by release version")
    parser.add_argument("version", help="Release version tag, e.g. v0.3.0")
    parser.add_argument("--dry-run", action="store_true", help="Show what would happen without making changes")
    args = parser.parse_args()

    if not validate_version(args.version):
        print(f"Error: version must be in the form vX.Y.Z, got: {args.version}", file=sys.stderr)
        sys.exit(1)

    target_dir = os.path.join(CLOSED_DIR, args.version)

    md_files = [
        f for f in sorted(os.listdir(CLOSED_DIR))
        if f.endswith(".md") and os.path.isfile(os.path.join(CLOSED_DIR, f))
    ]

    if not md_files:
        print("No .md files found in the root closed/ directory — nothing to move.")
        return

    print(f"Target directory: {target_dir}")
    print(f"Files to move ({len(md_files)}):")
    for f in md_files:
        print(f"  {f}")

    if args.dry_run:
        print("Dry run — no changes made.")
        return

    os.makedirs(target_dir, exist_ok=True)

    for fname in md_files:
        src = os.path.join(CLOSED_DIR, fname)
        dst = os.path.join(target_dir, fname)
        shutil.move(src, dst)
        print(f"Moved: {fname}")

    script_dir = os.path.dirname(os.path.abspath(__file__))
    overview_script = os.path.join(script_dir, "update_task_overview.py")
    subprocess.run([sys.executable, overview_script], check=True)

    print(f"Done — {len(md_files)} tasks archived under {target_dir}")


if __name__ == "__main__":
    main()
