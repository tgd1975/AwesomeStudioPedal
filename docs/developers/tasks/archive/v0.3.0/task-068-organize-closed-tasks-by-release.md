---
id: TASK-068
title: Organize closed tasks by release version
status: closed
closed: 2026-04-16
opened: 2026-04-11
effort: Small (<2h)
complexity: Junior
human-in-loop: No
---

## Description

Create a system to organize closed tasks by release version. Upon each release, all tasks in `docs/developers/tasks/closed/` should be moved to a subdirectory named after the release version (e.g., `docs/developers/tasks/closed/v0.2.0/`). The task overview should ignore these subfolders and only show tasks in the root closed directory.

## Acceptance Criteria

- [ ] Create a script `scripts/organize_closed_tasks.py` that:
  - Takes a release version as argument (e.g., `v0.2.0`)
  - Creates a subdirectory `docs/developers/tasks/closed/<version>/`
  - Moves all `.md` files from `docs/developers/tasks/closed/` to the version subdirectory
  - Updates the task files to reflect they were completed in that release
- [ ] Modify `scripts/update_task_overview.py` to:
  - Only scan the root `closed/` directory (ignore version subdirectories)
  - Show version subdirectories as archived releases in the overview
- [ ] Add a release step to the release workflow that calls this script
- [ ] Test with a mock release to ensure it works correctly

## Notes

This will help track which tasks were completed in each release and keep the active closed tasks separate from archived ones.
