#!/usr/bin/env python3
"""
Regenerate the Future Ideas section in README.md from idea files
in docs/developers/tasks/future/.

Each file must be named IDEA-NNN-short-description.md and contain
YAML frontmatter with at least `id` and `title` fields.

Run this whenever an idea file is created or modified.
Usage: python scripts/update_future_ideas.py
"""
import os
import re

FUTURE_DIR = "docs/developers/ideas"
README = "README.md"

FRONTMATTER_RE = re.compile(r"^---\n(.*?)\n---", re.DOTALL)
FIELD_RE = re.compile(r"^(\w[\w-]*):\s*(.+)$", re.MULTILINE)
MARKER_START = "<!-- FUTURE IDEAS GENERATED -->"
MARKER_END = "<!-- END FUTURE IDEAS GENERATED -->"


def parse_idea_file(path):
    with open(path, encoding="utf-8") as f:
        content = f.read()
    m = FRONTMATTER_RE.match(content)
    if not m:
        return None
    fields = dict(FIELD_RE.findall(m.group(1)))
    fields["_file"] = os.path.basename(path)
    return fields


def load_ideas():
    ideas = []
    if not os.path.isdir(FUTURE_DIR):
        return ideas
    for fname in sorted(os.listdir(FUTURE_DIR)):
        if not fname.endswith(".md"):
            continue
        idea = parse_idea_file(os.path.join(FUTURE_DIR, fname))
        if idea:
            ideas.append(idea)
    return ideas


def main():
    ideas = load_ideas()

    lines = [
        "",
        "The following ideas are potential future enhancements for AwesomeStudioPedal.",
        "These are not committed features but represent directions the project could explore.",
        "Ideas are tracked in [`docs/developers/ideas/`](docs/developers/ideas/).",
        "",
    ]

    if ideas:
        for idea in ideas:
            idea_id = idea.get("id", "?")
            title = idea.get("title", idea["_file"])
            description = idea.get("description", "")
            fname = idea["_file"]
            entry = f"- **[{idea_id}](docs/developers/ideas/{fname}): {title}**"
            if description:
                entry += f" — {description}"
            lines.append(entry)
    else:
        lines.append("_No ideas tracked yet._")

    lines += [
        "",
        "These ideas are open for community contributions and discussions.",
        "If you're interested in working on any of these, please open an issue or start a discussion!",
    ]

    with open(README, encoding="utf-8") as f:
        content = f.read()

    start = content.find(MARKER_START)
    end = content.find(MARKER_END)

    if start == -1 or end == -1:
        print(f"ERROR: markers not found in {README}. Add {MARKER_START} / {MARKER_END} around the Future Ideas body.")
        return 1

    new_block = MARKER_START + "\n" + "\n".join(lines) + "\n" + MARKER_END
    content = content[:start] + new_block + content[end + len(MARKER_END):]

    with open(README, "w", encoding="utf-8") as f:
        f.write(content)

    print(f"Updated {README} ({len(ideas)} idea(s))")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
