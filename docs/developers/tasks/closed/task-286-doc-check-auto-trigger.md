---
id: TASK-286
title: Promote doc-check from advisory to auto-trigger on .md file moves
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Junior
human-in-loop: No
epic: agent-collaboration-skills
order: 4
---

## Description

The `/doc-check` skill already exists and implements the three-persona
routing the user asked for in chat `51ee7254-8c1a-4016-99c5-7f044559570f`.
It classifies `.md` files as builder/musician/developer based on path
plus content keywords.

Two practical issues remain:

1. **Under-triggered.** It was invoked only **2 times** across 148
   transcripts, despite many sessions involving `.md` writes under
   `docs/`. The skill description does not name the trigger condition
   the agent should react to.
2. **Advisory-only.** It reports mismatches but does not offer to move
   the file. The user's original phrasing ("ensure that documentation is
   placed to the right place") arguably implies enforcement.

This task fixes both: rewrite the skill description so the agent
auto-invokes it after creating or moving any `.md` under `docs/`, and
have the skill *offer* the suggested move (waiting for confirmation
before doing it).

## Acceptance Criteria

- [x] `doc-check/SKILL.md` description starts with a clear trigger:
      "After creating or moving any `.md` file under `docs/` (excluding
      `docs/developers/tasks/`), invoke this skill."
- [x] On a Mismatch verdict, the skill prints a proposed `git mv` command
      and asks the user to confirm before running it.
- [x] CLAUDE.md gains a one-line trigger note pointing at this skill.

## Test Plan

No automated tests required — primarily skill metadata and prose changes.
If the move-offer logic adds Python, unit-test it under `scripts/tests/`
following the existing housekeep test pattern.

## Notes

- Source signal: chat `51ee7254` and 2 invocations across 148 transcripts.
- The classification heuristic itself appears sound and is not in scope.
- Consider whether the skill should also flag *un-changed* misplaced files
  as a periodic audit mode (separate sub-task if pursued).
