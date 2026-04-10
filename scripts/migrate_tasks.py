#!/usr/bin/env python3
"""
Migrate tasks from REPOSITORY_IMPROVEMENT_CONCEPT.md into
docs/developers/tasks/{open,closed}/ and regenerate OVERVIEW.md.

Usage: python scripts/migrate_tasks.py
"""
import re
import os

CONCEPT_FILE = "REPOSITORY_IMPROVEMENT_CONCEPT.md"
TASKS_DIR = "docs/developers/tasks"
OPEN_DIR = os.path.join(TASKS_DIR, "open")
CLOSED_DIR = os.path.join(TASKS_DIR, "closed")

# Matches table rows like:
# | ✅ | TASK-001 | Description | 1 | S | No | Junior |
ROW_RE = re.compile(
    r"^\|\s*(✅|❌)\s*\|\s*(TASK-\d+)\s*\|\s*(.+?)\s*\|\s*(\d+)\s*\|\s*(\w+)\s*\|\s*(.+?)\s*\|\s*(.+?)\s*\|"
)

EFFORT_LABELS = {
    "S": "Small (<2h)",
    "M": "Medium (2-8h)",
    "L": "Large (8-24h)",
    "XL": "Extra Large (24-40h)",
}


def slug(task_id, description):
    clean = re.sub(r"[^a-z0-9]+", "-", description.lower()).strip("-")
    clean = re.sub(r"-+", "-", clean)[:50].rstrip("-")
    return f"{task_id}-{clean}"


def write_task_file(task):
    status = task["status"]
    target_dir = CLOSED_DIR if status == "closed" else OPEN_DIR
    filename = slug(task["id"].lower(), task["title"]) + ".md"
    path = os.path.join(target_dir, filename)

    effort_label = EFFORT_LABELS.get(task["effort"], task["effort"])

    content = f"""---
id: {task["id"]}
title: {task["title"]}
status: {status}
effort: {effort_label}
complexity: {task["complexity"]}
human-in-loop: {task["human"]}
---

## Description

{task["title"]}.

## Acceptance Criteria

- [ ] (fill in when picking up this task)

## Notes
"""
    with open(path, "w") as f:
        f.write(content)
    return filename, target_dir


def generate_overview(tasks):
    open_tasks = [t for t in tasks if t["status"] == "open"]
    closed_tasks = [t for t in tasks if t["status"] == "closed"]

    lines = ["# Task Overview\n",
             "_This file is auto-generated. Do not edit manually._",
             "_Update it by running `python scripts/update_task_overview.py`._\n",
             f"**Open: {len(open_tasks)}** | **Closed: {len(closed_tasks)}** | **Total: {len(tasks)}**\n",
             "## Open Tasks\n",
             "| ID | Title | Effort | Complexity |",
             "|----|-------|--------|------------|"]

    for t in open_tasks:
        fname = slug(t["id"].lower(), t["title"]) + ".md"
        effort_label = EFFORT_LABELS.get(t["effort"], t["effort"])
        lines.append(
            f"| [{t['id']}](open/{fname}) | {t['title']} | {effort_label} | {t['complexity']} |"
        )

    lines += ["\n## Closed Tasks\n",
              "| ID | Title | Effort |",
              "|----|-------|--------|"]

    for t in closed_tasks:
        fname = slug(t["id"].lower(), t["title"]) + ".md"
        effort_label = EFFORT_LABELS.get(t["effort"], t["effort"])
        lines.append(
            f"| [{t['id']}](closed/{fname}) | {t['title']} | {effort_label} |"
        )

    path = os.path.join(TASKS_DIR, "OVERVIEW.md")
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")
    print(f"Written {path}")


def main():
    tasks = []
    with open(CONCEPT_FILE) as f:
        for line in f:
            m = ROW_RE.match(line)
            if not m:
                continue
            status_icon, task_id, title, _phase, effort, human, complexity = m.groups()
            # Strip markdown bold/inline code from title
            title = re.sub(r"`[^`]*`", lambda x: x.group().strip("`"), title)
            title = title.replace("**", "")
            tasks.append({
                "id": task_id,
                "title": title.strip(),
                "status": "closed" if status_icon == "✅" else "open",
                "effort": effort.strip(),
                "complexity": complexity.strip(),
                "human": human.strip(),
            })

    print(f"Found {len(tasks)} tasks")
    for task in tasks:
        fname, d = write_task_file(task)
        print(f"  {task['status']:6s}  {task['id']}  →  {d}/{fname}")

    generate_overview(tasks)


if __name__ == "__main__":
    main()
