#!/usr/bin/env python3
"""Sync enabled_skills in .vibe/config.toml with .claude/skills/ directory.

Scans every subdirectory of .claude/skills/ that contains a SKILL.md file
and rewrites the enabled_skills line in .vibe/config.toml to match.

Exit codes: 0 = success, 1 = error.
"""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SKILLS_DIR = ROOT / ".claude" / "skills"
CONFIG_FILE = ROOT / ".vibe" / "config.toml"


def get_installed_skills() -> list[str]:
    if not SKILLS_DIR.is_dir():
        return []
    return sorted(
        d.name for d in SKILLS_DIR.iterdir()
        if d.is_dir() and (d / "SKILL.md").exists()
    )


def update_config(skills: list[str]) -> None:
    if not CONFIG_FILE.exists():
        print(f"ERROR: {CONFIG_FILE} not found", file=sys.stderr)
        sys.exit(1)

    content = CONFIG_FILE.read_text()
    value = "[" + ", ".join(f'"{s}"' for s in skills) + "]"
    new_line = f"enabled_skills = {value}"
    updated = re.sub(
        r"^enabled_skills\s*=\s*\[.*?\]",
        new_line,
        content,
        flags=re.MULTILINE,
    )
    if updated == content and new_line not in content:
        print("ERROR: enabled_skills line not found in config", file=sys.stderr)
        sys.exit(1)

    CONFIG_FILE.write_text(updated)


if __name__ == "__main__":
    skills = get_installed_skills()
    update_config(skills)
    print(f"Synced {len(skills)} skill(s): {', '.join(skills)}")
