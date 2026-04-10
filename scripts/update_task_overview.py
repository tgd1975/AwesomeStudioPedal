#!/usr/bin/env python3
"""
Regenerate docs/developers/tasks/OVERVIEW.md from the task files
in docs/developers/tasks/{open,closed}/.

Run this whenever a task is created, moved, or modified.
Usage: python scripts/update_task_overview.py
"""
import os
import re

TASKS_DIR = "docs/developers/tasks"
OPEN_DIR = os.path.join(TASKS_DIR, "open")
CLOSED_DIR = os.path.join(TASKS_DIR, "closed")
OVERVIEW = os.path.join(TASKS_DIR, "OVERVIEW.md")

FRONTMATTER_RE = re.compile(r"^---\n(.*?)\n---", re.DOTALL)
FIELD_RE = re.compile(r"^(\w[\w-]*):\s*(.+)$", re.MULTILINE)
MARKER_START = "<!-- GENERATED -->"
MARKER_END = "<!-- END GENERATED -->"


def parse_task_file(path):
    with open(path) as f:
        content = f.read()
    m = FRONTMATTER_RE.match(content)
    if not m:
        return None
    fields = dict(FIELD_RE.findall(m.group(1)))
    fields["_file"] = os.path.basename(path)
    return fields


def load_tasks(directory, status):
    tasks = []
    if not os.path.isdir(directory):
        return tasks
    for fname in sorted(os.listdir(directory)):
        if not fname.endswith(".md"):
            continue
        task = parse_task_file(os.path.join(directory, fname))
        if task:
            task.setdefault("status", status)
            tasks.append(task)
    return tasks


def read_frame():
    """Return (prefix, suffix) — the static content before and after the generated block."""
    if not os.path.exists(OVERVIEW):
        return MARKER_START + "\n", "\n" + MARKER_END + "\n"
    with open(OVERVIEW) as f:
        content = f.read()
    start = content.find(MARKER_START)
    end = content.find(MARKER_END)
    if start == -1:
        return content.rstrip("\n") + "\n\n" + MARKER_START + "\n", "\n" + MARKER_END + "\n"
    prefix = content[: start + len(MARKER_START)] + "\n"
    suffix = "\n" + MARKER_END + (content[end + len(MARKER_END):] if end != -1 else "\n")
    return prefix, suffix


def main():
    open_tasks = load_tasks(OPEN_DIR, "open")
    closed_tasks = load_tasks(CLOSED_DIR, "closed")

    prefix, suffix = read_frame()

    lines = [
        "",
        f"**Open: {len(open_tasks)}** | **Closed: {len(closed_tasks)}** | **Total: {len(open_tasks) + len(closed_tasks)}**",
        "",
        "## Open Tasks",
        "",
        "| ID | Title | Effort | Complexity |",
        "|----|-------|--------|------------|",
    ]

    for t in open_tasks:
        task_id = t.get("id", "?")
        title = t.get("title", t["_file"])
        effort = t.get("effort", "?")
        complexity = t.get("complexity", "?")
        lines.append(f"| [{task_id}](open/{t['_file']}) | {title} | {effort} | {complexity} |")

    lines += [
        "",
        "## Closed Tasks",
        "",
        "| ID | Title | Effort |",
        "|----|-------|--------|",
    ]

    for t in closed_tasks:
        task_id = t.get("id", "?")
        title = t.get("title", t["_file"])
        effort = t.get("effort", "?")
        lines.append(f"| [{task_id}](closed/{t['_file']}) | {title} | {effort} |")

    with open(OVERVIEW, "w") as f:
        f.write(prefix + "\n".join(lines) + suffix)

    print(f"Updated {OVERVIEW} ({len(open_tasks)} open, {len(closed_tasks)} closed)")


if __name__ == "__main__":
    main()
