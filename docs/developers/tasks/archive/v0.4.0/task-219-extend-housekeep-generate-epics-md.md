---
id: TASK-219
title: Extend `housekeep.py` to generate `EPICS.md` (dependency graph default, Gantt as config option)
status: closed
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 12
prerequisites: [TASK-218]
---

## Description

Extend `housekeep.py` to generate a full `EPICS.md` file. The default visualization is a Mermaid dependency graph derived from `prerequisites:` relationships between tasks. A Gantt chart alternative is available via `visualizations.epics.style: gantt` in config.

Each epic gets its own section with:

- Epic title and status
- Task dependency graph (or Gantt if configured)
- Graceful degradation to flat list when no prerequisites exist

## Acceptance Criteria

- [ ] `housekeep.py --apply` generates `EPICS.md` with one section per epic
- [ ] Default style is dependency graph — Mermaid `graph TD` using `prerequisites:` fields
- [ ] `visualizations.epics.style: gantt` produces a Mermaid `gantt` chart instead
- [ ] Epics with no prerequisite relationships produce a flat task list (not a broken graph)
- [ ] `visualizations.epics.enabled: false` skips `EPICS.md` generation entirely
- [ ] `tasks.epics.enabled: false` also skips `EPICS.md` (takes precedence)

## Test Plan

**Host tests:** Extend `test/unit/test_housekeep.py` — cover: epic with prerequisites produces correct graph edges, epic without prerequisites produces flat list, Gantt mode produces gantt block, disabled via config skips generation.

## Prerequisites

- **TASK-218** — config system and `assigned` field wiring must be complete before extending overview generation

## Notes

Clarification needed: for Gantt mode, what should be used as the task duration when `effort` maps to a date range? Suggested mapping: Small=2d, Medium=5d, Large=12d, Extra Large=20d.
