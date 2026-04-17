---
name: task-done
description: Mark a task as closed — moves its file to closed/ and regenerates OVERVIEW.md
---

# task-done

The user invokes this as `/task-done TASK-NNN` (or with a partial ID like `47`).

Steps:

0. Run `/check-branch` to verify the current branch is not `main` before proceeding.
1. Find the matching file in `docs/developers/tasks/open/` whose name contains the
   given task ID (case-insensitive, e.g. `task-047`).
2. If not found, report the error and stop.
3. Update the `status:` field in the file's frontmatter from `open` to `closed`.
   Add `closed: <today's date as YYYY-MM-DD>` on the line after `status: closed`.
4. Move the file to `docs/developers/tasks/closed/` using `git mv`.
5. Run `python scripts/update_task_overview.py` to regenerate `OVERVIEW.md`.
6. Stage the changed files (`git add`) and create a commit with the message:
   `close TASK-NNN: <task title from frontmatter>`
   Do NOT push.
7. Report: "TASK-NNN moved to closed, OVERVIEW.md updated, and committed."
