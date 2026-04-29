#!/usr/bin/env python3
"""
Regenerate docs/developers/ideas/OVERVIEW.md from idea files in
docs/developers/ideas/open/.

Each file must contain YAML frontmatter with at least `id` and `title`
fields; `description` and `category` are optional. A missing `category`
renders as an em dash in the table.

Usage:
    python scripts/update_idea_overview.py            # write OVERVIEW.md
    python scripts/update_idea_overview.py --dry-run  # print to stdout
"""
import argparse
import os
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import task_system_config as tsc

_CFG = tsc.load(warn=False)
IDEAS_DIR = tsc.get(_CFG, "ideas", "base_folder", default="docs/developers/ideas")
OPEN_DIR = os.path.join(IDEAS_DIR, "open")
ARCHIVED_DIR = os.path.join(IDEAS_DIR, "archived")
OVERVIEW = os.path.join(IDEAS_DIR, "OVERVIEW.md")

FRONTMATTER_RE = re.compile(r"^---\n(.*?)\n---", re.DOTALL)
FIELD_RE = re.compile(r"^(\w[\w-]*):\s*(.+)$", re.MULTILINE)


def parse_idea_file(path):
    with open(path, encoding="utf-8") as f:
        content = f.read()
    m = FRONTMATTER_RE.match(content)
    if not m:
        return None
    fields = dict(FIELD_RE.findall(m.group(1)))
    fields["_file"] = os.path.basename(path)
    return fields


def load_ideas(directory):
    ideas = []
    if not os.path.isdir(directory):
        return ideas
    for fname in sorted(os.listdir(directory)):
        if not fname.endswith(".md"):
            continue
        idea = parse_idea_file(os.path.join(directory, fname))
        if idea:
            ideas.append(idea)
    return ideas


def render_overview(open_ideas, archived_ideas):
    lines = [
        "# Ideas Overview",
        "",
        f"**Open: {len(open_ideas)}** | **Archived: {len(archived_ideas)}**",
        "",
        "Ideas are lightweight, qualitative proposals tracked in"
        " [`open/`](open/) until they are either converted into structured"
        " tasks or archived. Archived ideas are kept for history in"
        " [`archived/`](archived/).",
        "",
        "## Open Ideas",
        "",
    ]

    if open_ideas:
        lines += [
            "| ID | Category | Title | Description |",
            "|----|----------|-------|-------------|",
        ]
        for idea in open_ideas:
            idea_id = idea.get("id", "?")
            title = idea.get("title", idea["_file"])
            description = idea.get("description", "").replace("|", "\\|")
            category = idea.get("category", "").strip() or "—"
            fname = idea["_file"]
            lines.append(
                f"| [{idea_id}](open/{fname}) | {category} | {title} | {description} |"
            )
    else:
        lines.append("_No open ideas._")

    if archived_ideas:
        lines += [
            "",
            "## Archived Ideas",
            "",
            "| ID | Category | Title |",
            "|----|----------|-------|",
        ]
        for idea in archived_ideas:
            idea_id = idea.get("id", "?")
            title = idea.get("title", idea["_file"])
            category = idea.get("category", "").strip() or "—"
            fname = idea["_file"]
            lines.append(f"| [{idea_id}](archived/{fname}) | {category} | {title} |")

    return "\n".join(lines) + "\n"


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[1] if __doc__ else "")
    parser.add_argument("--dry-run", action="store_true",
                        help="Print generated OVERVIEW.md to stdout without writing.")
    args = parser.parse_args(argv)

    open_ideas = load_ideas(OPEN_DIR)
    archived_ideas = load_ideas(ARCHIVED_DIR)
    output = render_overview(open_ideas, archived_ideas)

    if args.dry_run:
        sys.stdout.write(output)
        return 0

    with open(OVERVIEW, "w", encoding="utf-8") as f:
        f.write(output)
    print(f"Updated {OVERVIEW} ({len(open_ideas)} open, {len(archived_ideas)} archived)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
