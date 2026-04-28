---
id: TASK-221
title: Implement `ts-epic-list` skill — list all epics with derived status, assigned owner, task counts
status: closed
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 14
prerequisites: [TASK-215]
---

## Description

Create the `ts-epic-list` skill that displays all epics (open, active, closed) with their derived status, assigned owner, and open/active/closed task counts. This completes the triad of list commands — `/ts-task-list`, `/ts-idea-list`, `/ts-epic-list` — giving full project visibility from the CLI without opening generated files.

## Acceptance Criteria

- [ ] `.claude/skills/ts-epic-list/SKILL.md` exists
- [ ] Displays each epic: ID, title, status, assigned (if set), open/active/closed task counts
- [ ] Reads directly from epic files across `open/`, `active/`, `closed/` folders
- [ ] Skill is registered in `.vibe/config.toml`
- [ ] Output is concise — one line per epic

## Test Plan

**Host tests:** No isolated logic — manual smoke test with epics in different states.

## Prerequisites

- **TASK-215** — epic files must exist before they can be listed

## Notes

Clarification needed: should the skill read from `EPICS.md` (generated file) or directly from the epic source files? Recommendation: read source files directly — the generated file can be stale if `housekeep.py` hasn't been run.
