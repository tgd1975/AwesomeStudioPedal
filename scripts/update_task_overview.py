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
ARCHIVE_DIR = os.path.join(TASKS_DIR, "archive")
OVERVIEW = os.path.join(TASKS_DIR, "OVERVIEW.md")

FRONTMATTER_RE = re.compile(r"^---\n(.*?)\n---", re.DOTALL)
FIELD_RE = re.compile(r"^(\w[\w-]*):\s*(.+)$", re.MULTILINE)
MARKER_START = "<!-- GENERATED -->"
MARKER_END = "<!-- END GENERATED -->"


def parse_task_file(path):
    with open(path, encoding='utf-8') as f:
        content = f.read()
    m = FRONTMATTER_RE.match(content)
    if not m:
        return None
    fields = dict(FIELD_RE.findall(m.group(1)))
    fields["_file"] = os.path.basename(path)
    return fields


def load_tasks(directory, status):
    """Load tasks from the root of directory only (ignores subdirectories)."""
    tasks = []
    if not os.path.isdir(directory):
        return tasks
    for fname in sorted(os.listdir(directory)):
        if not fname.endswith(".md"):
            continue
        path = os.path.join(directory, fname)
        if not os.path.isfile(path):
            continue
        task = parse_task_file(path)
        if task:
            task.setdefault("status", status)
            tasks.append(task)
    return tasks


def load_archived_releases(archive_dir):
    """Return sorted list of version subdirectory names under archive/ (e.g. ['v0.1.0', 'v0.2.0'])."""
    if not os.path.isdir(archive_dir):
        return []
    return sorted(
        entry for entry in os.listdir(archive_dir)
        if os.path.isdir(os.path.join(archive_dir, entry))
        and re.fullmatch(r"v\d+\.\d+\.\d+", entry)
    )


def generate_release_overview(version, release_dir):
    """Write closed/<version>/OVERVIEW.md listing every task in that release folder."""
    tasks = load_tasks(release_dir, "closed")
    tasks.sort(key=lambda t: t.get("id", ""))

    lines = [
        f"# Tasks archived in {version}",
        "",
        f"**{len(tasks)} task(s) closed in this release.**",
        "",
        "| ID | Title | Effort | Complexity |",
        "|----|-------|--------|------------|",
    ]
    for t in tasks:
        task_id = t.get("id", "?")
        title = t.get("title", t["_file"])
        effort = t.get("effort", "?")
        complexity = t.get("complexity", "?")
        lines.append(f"| [{task_id}]({t['_file']}) | {title} | {effort} | {complexity} |")

    out_path = os.path.join(release_dir, "OVERVIEW.md")
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")
    return out_path


def generate_release_overviews(archive_dir, versions):
    """Generate OVERVIEW.md for every archived release version."""
    generated = []
    for version in versions:
        release_dir = os.path.join(archive_dir, version)
        out_path = generate_release_overview(version, release_dir)
        generated.append(out_path)
    return generated


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
    archived_releases = load_archived_releases(ARCHIVE_DIR)

    prefix, suffix = read_frame()

    # Split open tasks into grouped and ungrouped
    ungrouped = [t for t in open_tasks if not t.get("group")]

    # Build group index from ALL tasks (open + closed) so groups show their full history.
    # Key: group name → list of tasks (mixed statuses), sorted by order.
    all_grouped: dict[str, list] = {}
    for t in open_tasks + closed_tasks:
        g = t.get("group")
        if g:
            all_grouped.setdefault(g, []).append(t)
    for g in all_grouped:
        all_grouped[g].sort(key=lambda t: int(t.get("order", 999)))

    # Only emit groups that have at least one open task
    active_groups = {g: tasks for g, tasks in all_grouped.items()
                     if any(t.get("status") == "open" for t in tasks)}

    lines = [
        "",
        f"**Open: {len(open_tasks)}** | **Closed: {len(closed_tasks)}** | **Total: {len(open_tasks) + len(closed_tasks)}**",
        "",
        "## Open Tasks",
        "",
        "### All Open Tasks",
    ]

    def task_table_rows(tasks):
        rows = []
        for t in tasks:
            task_id = t.get("id", "?")
            title = t.get("title", t["_file"])
            effort = t.get("effort", "?")
            complexity = t.get("complexity", "?")
            rows.append(f"| [{task_id}](open/{t['_file']}) | {title} | {effort} | {complexity} |")
        return rows

    if open_tasks:
        lines += [
            "",
            "| ID | Title | Effort | Complexity |",
            "|----|-------|--------|------------|",
        ]
        lines += task_table_rows(sorted(open_tasks, key=lambda t: t.get("id", "")))
    else:
        lines += ["", "_No open tasks._"]

    lines += [
        "",
        "## Task Groups",
        "",
        "Groups with at least one open task are shown below."
        " Each group lists all its tasks — open and closed."
        " Closed tasks are ~~struck through~~.",
        "_If no sub-sections appear here, there are currently no open tasks assigned to a group._",
    ]

    for group_name, tasks in sorted(active_groups.items()):
        lines += [
            "",
            f"### {group_name}",
            "",
            "| Order | ID | Title | Effort | Complexity | Human-in-loop |",
            "|-------|----|-------|--------|------------|---------------|",
        ]
        for t in tasks:
            task_id = t.get("id", "?")
            title = t.get("title", t["_file"])
            effort = t.get("effort", "?")
            complexity = t.get("complexity", "?")
            hil = t.get("human-in-loop", "?")
            if hil == "Main":
                hil += " ★"
            order = t.get("order", "?")
            is_closed = t.get("status") == "closed"
            subdir = "closed" if is_closed else "open"
            if is_closed:
                lines.append(
                    f"| {order} | ~~[{task_id}]({subdir}/{t['_file']})~~ |"
                    f" ~~{title}~~ | ~~{effort}~~ | ~~{complexity}~~ | ~~{hil}~~ |"
                )
            else:
                lines.append(
                    f"| {order} | [{task_id}]({subdir}/{t['_file']}) |"
                    f" {title} | {effort} | {complexity} | {hil} |"
                )

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

    if archived_releases:
        lines += [
            "",
            "## Archived Releases",
            "",
        ]
        for version in archived_releases:
            lines.append(f"- [{version}](archive/{version}/OVERVIEW.md)")

    with open(OVERVIEW, "w") as f:
        f.write(prefix + "\n".join(lines) + suffix)

    # Generate per-release OVERVIEW.md files
    release_overviews = generate_release_overviews(ARCHIVE_DIR, archived_releases)

    print(f"Updated {OVERVIEW} ({len(open_tasks)} open, {len(closed_tasks)} closed, {len(archived_releases)} archived releases)")
    for p in release_overviews:
        print(f"  Generated {p}")


if __name__ == "__main__":
    main()
