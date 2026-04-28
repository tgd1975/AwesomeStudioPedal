---
id: TASK-214
title: Create `ts-task-active`, `ts-task-pause`, and `ts-task-reopen` skills
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 7
prerequisites: [TASK-213]
---

## Description

Implement three new task lifecycle skills that manage the `active` state:

- **`ts-task-active TASK-NNN`** — sets `status: active`, moves task to `active/`, runs `housekeep.py --apply`. Does not commit.
- **`ts-task-pause TASK-NNN`** — sets `status: open`, moves task back to `open/`, runs `housekeep.py --apply`. Does not commit.
- **`ts-task-reopen TASK-NNN`** — sets `status: open`, moves task from `closed/` to `open/`, runs `housekeep.py --apply`. Does not commit.

Register all three in `.vibe/config.toml` under `enabled_skills`.

## Acceptance Criteria

- [ ] `.claude/skills/ts-task-active/SKILL.md` exists and moves task to `active/`
- [ ] `.claude/skills/ts-task-pause/SKILL.md` exists and moves task back to `open/`
- [ ] `.claude/skills/ts-task-reopen/SKILL.md` exists and moves task from `closed/` to `open/`
- [ ] All three skills run `housekeep.py --apply` after the status change
- [ ] All three are registered in `.vibe/config.toml`
- [ ] Skills handle the case where `tasks.active.enabled: false` — either warn or treat `active` as `open`

## Test Plan

**Host tests:** Skills contain no testable logic — manual smoke test each state transition.

## Prerequisites

- **TASK-213** — `housekeep.py` must exist before skills can invoke it

## Notes

Clarification needed: when `tasks.active.enabled: false`, should `ts-task-active` be disabled (removed from `enabled_skills`) or should it silently treat the task as `open`?
