---
id: TASK-254
title: Enforce active status for paused tasks blocked by defects
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Clarification
---

## Description

Today the workflow allows trivial tasks to be completed in one go without ever
transitioning through `active` — and that is fine. However, when a task **starts**
and then has to be **paused** (e.g. because executing it surfaced a defect that must
be fixed first, as happened with TASK-153 and TASK-155), it must be in `active`
status before being paused. The current `/ts-task-pause` skill moves a task from
`active/` back to `open/`, which loses the signal that work was already begun and
is now blocked.

This task tightens the rule and updates the skills + ticket-system docs so that:

1. A task that is paused mid-execution remains in `active/` (not moved back to
   `open/`), with status reflecting it is paused/blocked.
2. The implicit "skip active for one-shot tasks" path stays allowed for tasks that
   complete in a single sitting without being interrupted.
3. The documentation (`docs/developers/TASK_SYSTEM.md`) and the skills
   (`ts-task-pause`, `ts-task-active`, possibly `ts-task-new`) describe and enforce
   this distinction.

## Acceptance Criteria

- [x] `docs/developers/TASK_SYSTEM.md` documents the rule: paused-because-blocked
      tasks must be `active`; one-shot tasks may bypass `active` entirely.
- [x] `/ts-task-pause` skill is updated so paused tasks stay in `active/` (or its
      behaviour is otherwise reconciled with the new rule — see Notes).
- [x] `task-system.yaml` (and any housekeep / status-derivation logic in
      `scripts/housekeep.py`) reflects the updated state model. **Resolved without
      schema or code changes** — see Resolution below.
- [x] OVERVIEW.md / KANBAN.md regeneration still produces sensible output for the
      paused-while-active case after running `python scripts/housekeep.py --apply`.

## Resolution

The user's design decision (2026-04-26): keep the existing three statuses
(`open` / `active` / `closed`); do **not** introduce a `paused` status or
`paused:` flag. A paused task stays `active`, and the pause is recorded by:

1. Listing the blocking task ID in `prerequisites:` in the frontmatter
   (the field already exists and feeds the epic dependency graph).
2. Adding a short `## Paused` section to the task body with the date and
   the reason ("waiting on TASK-XYZ because …").

This needed no changes to `task-system.yaml` or `scripts/housekeep.py`.
Only docs and the two skills were updated:

- `.claude/skills/ts-task-pause/SKILL.md` — rewritten: prompts for
  blocker IDs, edits frontmatter `prerequisites:` and appends a `## Paused`
  body section, **does not** change status or move the file.
- `.claude/skills/ts-task-active/SKILL.md` — extended: when invoked on a
  task already in `active/` whose `prerequisites:` are now closed, treats
  the call as a resume — clears the closed prereqs and the `## Paused`
  section.
- `docs/developers/TASK_SYSTEM.md` — updated lifecycle diagram, skills
  table, and added a "Pausing a task" subsection plus an explicit note on
  when `active` is mandatory vs. optional.

## Test Plan

No automated tests required — change is non-functional (docs, skill instructions,
and a Python housekeep script). Verify by:

- Running `python scripts/housekeep.py --apply` after the changes and confirming
  OVERVIEW/KANBAN render correctly.
- Manually walking a sample task through: `/ts-task-active` → `/ts-task-pause` →
  confirming it stays in `active/` with the expected status.

## Documentation

- `docs/developers/TASK_SYSTEM.md` — describe when `active` is mandatory vs.
  optional, and what "paused" means in the lifecycle diagram.
- `docs/developers/task-system.yaml` — update state list / transitions if a new
  `paused` status is introduced.
- `.claude/skills/ts-task-pause/SKILL.md` — change destination folder + status
  semantics.
- `.claude/skills/ts-task-active/SKILL.md` — clarify that resuming a paused task
  is a no-op move (already in `active/`) but should update status.

## Notes

- **Open design question for the user:** should "paused" be a distinct status
  value (`status: paused`) alongside `open`/`active`/`closed`, or should it remain
  `active` with a `paused: true` flag (or a `blocked-by:` field pointing at the
  defect task)? Recommend asking before implementing — this affects the YAML
  schema and the housekeep logic.
- TASK-153 and TASK-155 are real-world examples of the gap this addresses; both
  surfaced app defects mid-execution and were paused without ever being `active`.
- Keep the one-shot bypass path: forcing every trivial task through `active` would
  add ceremony with no value. The rule only bites when a task is interrupted.
