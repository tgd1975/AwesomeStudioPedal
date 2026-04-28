---
id: TASK-135
title: Delete idea-006 After Group C
status: closed
closed: 2026-04-19
opened: 2026-04-17
effort: Trivial (<30m)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
epic: Cleanup
order: 2
prerequisites: [TASK-114]
---

## Description

Once Group C (Macros) is fully implemented and host-tested, delete the originating idea file.

## Acceptance Criteria

- [ ] `docs/developers/tasks/future/idea-006-macros.md` deleted
- [ ] No other files reference this idea file as a dependency

## Files to Touch

- `docs/developers/tasks/future/idea-006-macros.md` (delete)

## Test Plan

`grep -r "idea-006" docs/` returns no results after deletion.

## Prerequisites

- **TASK-114** — Group C host tests must pass, confirming the macro feature is complete
