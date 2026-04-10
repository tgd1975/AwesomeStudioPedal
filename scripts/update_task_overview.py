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

EFFORT_ORDER = {
    "Small (<2h)": 0,
    "Medium (2-8h)": 1,
    "Large (8-24h)": 2,
    "Extra Large (24-40h)": 3,
}


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


def main():
    open_tasks = load_tasks(OPEN_DIR, "open")
    closed_tasks = load_tasks(CLOSED_DIR, "closed")

    lines = [
        "# Task Overview\n",
        "_This file is auto-generated. Do not edit manually._",
        "_Update it by running `python scripts/update_task_overview.py`._\n",
        f"**Open: {len(open_tasks)}** | **Closed: {len(closed_tasks)}** | **Total: {len(open_tasks) + len(closed_tasks)}**\n",
        "## Open Tasks\n",
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
        "\n## Closed Tasks\n",
        "| ID | Title | Effort |",
        "|----|-------|--------|",
    ]

    for t in closed_tasks:
        task_id = t.get("id", "?")
        title = t.get("title", t["_file"])
        effort = t.get("effort", "?")
        lines.append(f"| [{task_id}](closed/{t['_file']}) | {title} | {effort} |")

    with open(OVERVIEW, "w") as f:
        f.write("\n".join(lines) + "\n")

    print(f"Updated {OVERVIEW} ({len(open_tasks)} open, {len(closed_tasks)} closed)")


if __name__ == "__main__":
    main()
