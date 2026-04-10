---
name: task-new
description: Scaffold a new task file in docs/developers/tasks/open/ and update OVERVIEW.md
---

# task-new

The user invokes this as `/task-new "Short task title"` (optionally with
`--effort S|M|L|XL` and `--complexity Junior|Medium|Senior`).

Steps:

1. Determine the next task ID by scanning all files in both `open/` and `closed/`
   directories, finding the highest existing TASK-NNN number, and adding 1.
2. Build the filename slug from the title: lowercase, spaces/special chars → hyphens,
   max 50 chars, prefixed with the new ID (e.g. `task-059-my-new-task.md`).
3. Write the file to `docs/developers/tasks/open/` with this frontmatter template:

```markdown
---
id: TASK-NNN
title: <title>
status: open
effort: <effort label or "?">
complexity: <complexity or "?">
human-in-loop: ?
---

## Description

<title>.

## Acceptance Criteria

- [ ] (fill in when picking up this task)

## Notes
```

1. Run `python scripts/update_task_overview.py` to regenerate `OVERVIEW.md`.
1. Report the new task ID and file path.

Effort labels: S → "Small (<2h)", M → "Medium (2-8h)", L → "Large (8-24h)", XL → "Extra Large (24-40h)".
Do not commit.
