---
id: TASK-216
title: Update `ts-task-list` to show `active` state and read from both `open/` and `active/` folders
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 9
prerequisites: [TASK-214]
---

## Description

Update the task listing skill to read from both `open/` and `active/` folders and display the `status` field per task. Tasks in `active/` should be visually distinguished (e.g. bold, badge, or status column value). The skill should group tasks by epic and mark `human-in-loop: Main` entries with ★.

## Acceptance Criteria

- [ ] Skill reads tasks from both `open/` and `active/` folders
- [ ] `status` column shown in output (open / active)
- [ ] Active tasks are visually distinguishable from open tasks
- [ ] Grouping by epic still works across both folders
- [ ] `assigned` field shown if present
- [ ] `human-in-loop: Main` tasks marked with ★

## Test Plan

**Host tests:** No isolated logic — manual smoke test with at least one task in each state.

## Prerequisites

- **TASK-214** — `active/` folder and state must exist before the listing skill can read from it

## Notes

Clarification needed: should the skill fall back gracefully when `tasks.active.enabled: false` and the `active/` folder does not exist?
