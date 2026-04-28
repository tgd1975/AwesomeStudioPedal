---
id: TASK-213
title: Write `scripts/housekeep.py` — file moves, epic status derivation, overview regeneration, `--apply` flag
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Large (8-24h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Senior
human-in-loop: Support
epic: task-system
order: 6
prerequisites: [TASK-212]
---

## Description

Write the central housekeeping script that replaces `update_task_overview.py`. This is the engine of the task system: it moves task and epic files to the correct folder based on their `status` field, derives epic status from their tasks, and regenerates all overview files.

Key behaviours:

- Default mode is **dry-run** — prints what it would do but changes nothing
- `--apply` flag actually moves files and regenerates overviews
- Scans `open/`, `active/`, `closed/`, `archive/` for task files
- Reads `status:` frontmatter and moves files to the matching folder
- Derives epic status: all-closed → `closed`, any-active → `active`, otherwise → `open`
- Regenerates `OVERVIEW.md` (task list grouped by epic)
- Stubs for `EPICS.md` and `KANBAN.md` (full generation added in Phase 5)

## Acceptance Criteria

- [ ] `scripts/housekeep.py` exists with `--apply` / dry-run behaviour
- [ ] Correctly moves a task with `status: active` from `open/` to `active/`
- [ ] Correctly moves a task with `status: closed` from `open/` to `closed/`
- [ ] Derives epic status from task statuses and moves epic file accordingly
- [ ] Regenerates `OVERVIEW.md` after any file moves
- [ ] Dry-run output clearly lists what would change without changing anything
- [ ] Replaces `update_task_overview.py` (old script removed or deprecated)

## Test Plan

**Host tests:** Add `test/unit/test_housekeep.py` — cover: task in wrong folder, epic status derivation (all-open, mixed, all-closed), no tasks for epic, dry-run produces no side effects.

## Prerequisites

- **TASK-212** — complete idea system (Phase 2) should be done before housekeep.py is written, as housekeep.py will eventually govern both systems under a unified config

## Notes

Support role: consult on edge cases (files with invalid `status:`, missing frontmatter, orphaned epic files). The `active/` folder handling depends on `tasks.active.enabled` config — implement the config system minimally here, fully in Phase 4 (TASK-217).
