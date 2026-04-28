---
id: TASK-272
title: Soft nudge to split L/XL tasks at scaffold time
status: closed
closed: 2026-04-27
opened: 2026-04-26
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Clarification
epic: task-system-rollout
order: 5
---

## Description

Add a soft, non-blocking nudge to `ts-task-new` that fires when a new task is
scaffolded with `effort: L` or `effort: XL`. The skill should propose a candidate
split (2–4 smaller tasks, each ideally S or M) alongside the original, and let
the user choose one of three paths:

- **Accept split** → scaffold the smaller tasks instead, chained via
  `prerequisites`, sharing the same `epic` and contiguous `order` values.
- **Keep whole** → ask for a one-line reason and write it into a new
  `## Sizing rationale` section in the task body.
- **Skip nudge** (genuinely exploratory scope) → automatically record
  `Sizing rationale: scope not yet decomposable`.

The goal is a gentle prompt to think about decomposition, not a gate. Sometimes
a Large task really is atomic (a refactor that breaks the build mid-way, a
third-party migration) — the rationale captures *why* the size is intentional so
future readers don't have to re-litigate the decision.

Scope is **scaffold time only**. No activation-time check, no housekeep
warning, no pre-commit gate, no schema change. Keep the surface small.

## Acceptance Criteria

- [x] `awesome-task-system/skills/ts-task-new/SKILL.md` has a new step (e.g.
      step 3a) that triggers only when `--effort L` or `--effort XL`, presents
      a candidate split alongside the original, and offers the three choices
      above.
- [x] The skill's task template documents the optional `## Sizing rationale`
      section, with guidance that it appears only when effort is L/XL **and**
      the user kept the task whole (or skipped the nudge).
- [x] `python scripts/sync_task_system.py --apply` mirrors the change to
      `.claude/skills/ts-task-new/SKILL.md`, and `--check` passes afterward.
- [x] Manual smoke check documented in the commit message: scaffolding an `S`
      task does not trigger the nudge; scaffolding an `L` task does.

## Test Plan

No automated tests required — change is non-functional (skill prose only).
Verify via manual smoke check: invoke `/ts-task-new "throwaway" --effort L` in
a scratch branch and confirm the nudge fires; invoke with `--effort S` and
confirm it does not.

## Notes

- Concept was discussed in conversation on 2026-04-26; see commit history for
  context once landed.
- Deliberately **out of scope** for v1: catching tasks whose effort is bumped
  L/XL after creation, and any housekeep/CI enforcement. Revisit only if the
  soft nudge proves insufficient in practice.
- Touches only `awesome-task-system/skills/ts-task-new/SKILL.md` plus the
  synced live copy. No changes to `housekeep.py`, frontmatter schema, or
  OVERVIEW/EPICS/KANBAN generation.
