---
id: TASK-209
title: Update `ts-task-new`, `update_task_overview.py`, and `ts-task-list` to use `epic` field
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 2
prerequisites: [TASK-208]
---

## Description

Update all tooling that reads or writes the `group:` field to use `epic:` instead. This covers the task scaffolding skill, the overview generator script, and the task listing skill. OVERVIEW.md column header should also be updated from "Group" to "Epic".

## Acceptance Criteria

- [ ] `task-new` skill (or `ts-task-new` if renamed) writes `epic:` in the frontmatter template
- [ ] `--group` flag renamed to `--epic` in the task-new skill
- [ ] `update_task_overview.py` (or `housekeep.py` when it exists) reads `epic:` field for grouping
- [ ] OVERVIEW.md column header shows "Epic" instead of "Group"
- [ ] `ts-task-list` (or existing task-list skill) groups by `epic:` field
- [ ] All inline documentation and skill help text updated

## Test Plan

**Host tests:** Run `make test-host` after updating any Python scripts to ensure existing tests pass.

## Prerequisites

- **TASK-208** — all existing task files must use `epic:` before tooling is changed, to avoid mixed-field states

## Notes

Clarification needed: confirm whether the existing `task-new` skill should be renamed to `ts-task-new` as part of this task or deferred to Phase 6 (shippable packaging).
