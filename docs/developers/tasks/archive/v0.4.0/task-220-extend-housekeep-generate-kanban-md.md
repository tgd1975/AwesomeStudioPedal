---
id: TASK-220
title: Extend `housekeep.py` to generate `KANBAN.md` with `assigned` badges and `Closed` column
status: closed
opened: 2026-04-23
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 13
prerequisites: [TASK-219]
---

## Description

Extend `housekeep.py` to generate a `KANBAN.md` using the Mermaid `kanban` diagram. The board shows three columns: Open, Active, and Closed. Each card shows the task ID and title. Cards for tasks with an `assigned:` field include a `@username` badge. Archived tasks (in `archive/vX.Y.Z/`) are not shown.

## Acceptance Criteria

- [ ] `housekeep.py --apply` generates `KANBAN.md` with Open / Active / Closed columns
- [ ] Each card uses `TASK-NNN[Title]` format
- [ ] Assigned tasks show `@username` badge: `TASK-NNN[Title @username]`
- [ ] Archived tasks are excluded from the board
- [ ] `visualizations.kanban.enabled: false` skips `KANBAN.md` generation
- [ ] Board renders correctly in GitHub Markdown preview

## Test Plan

**Host tests:** Extend `test/unit/test_housekeep.py` — cover: task in each state appears in correct column, assigned badge appears, archived task excluded.

## Prerequisites

- **TASK-219** — EPICS.md generation should be implemented first to establish the pattern for new visualization outputs

## Notes

Clarification needed: should the `Active` column be omitted entirely when `tasks.active.enabled: false`, or should it be shown empty?
