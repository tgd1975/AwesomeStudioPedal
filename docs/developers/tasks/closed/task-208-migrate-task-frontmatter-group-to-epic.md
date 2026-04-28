---
id: TASK-208
title: Migrate all existing task frontmatter from `group:` to `epic:`
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
epic: task-system
order: 1
---

## Description

Rename the `group:` field to `epic:` in every existing task file under `docs/developers/tasks/`. This is a pure find-and-replace across frontmatter — no logic changes, no reordering. The field name change is the prerequisite for all downstream tooling updates in Phase 1.

## Acceptance Criteria

- [ ] All task files in `open/`, `closed/`, and `archive/` have `group:` replaced with `epic:`
- [ ] No task file retains the old `group:` field
- [ ] `git grep 'group:' docs/developers/tasks/` returns no hits (excluding this task's own group field — which will also be renamed)
- [ ] OVERVIEW.md still renders correctly after the rename (existing script handles both field names or is updated in TASK-209)

## Test Plan

**Host tests:** No code logic changes — verify by grep after migration.

## Notes

Run a dry-run with `sed -n` or `grep` before applying to confirm scope. The migration can be done with a single `sed -i` command across all task files.
