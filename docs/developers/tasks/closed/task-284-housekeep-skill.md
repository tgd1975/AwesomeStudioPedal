---
id: TASK-284
title: Add /housekeep skill wrapping scripts/housekeep.py --apply
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Junior
human-in-loop: No
epic: agent-collaboration-skills
order: 2
---

## Description

`python scripts/housekeep.py --apply` was invoked **95+ times** as raw bash
across the transcript corpus (with various `tail -N` suffixes). The task-done
skills already invoke it internally, but it is also run standalone after
many other operations — manual idea moves, epic edits, status fixups,
ad-hoc OVERVIEW regen.

A small `/housekeep` skill would:

1. Make the canonical invocation discoverable (the agent stops guessing
   between `python` and `python3`, between various `tail -N` truncations).
2. Stage the regenerated index files (`OVERVIEW.md`, `EPICS.md`,
   `KANBAN.md`) so they ride along with the change that triggered them
   — closing the gap that the existing
   `feedback_overview_regen_in_status_commits` memory was created to plug.
3. Provide one place to evolve the regen workflow (e.g. add a
   `--check` mode wrapper, add a "show me what changed" diff).

## Acceptance Criteria

- [x] `.claude/skills/housekeep/SKILL.md` exists and is registered in
      `.vibe/config.toml` (per CLAUDE.md skill-registration rule).
- [x] Skill runs `python scripts/housekeep.py --apply`, reports the diff
      summary, and stages the regenerated index files.
- [x] CLAUDE.md updated with a one-liner: "After any task-system file
      change, invoke `/housekeep` rather than running the script directly."

## Test Plan

No automated tests required — change is non-functional (skill scaffolding
plus a CLAUDE.md prose addition).

## Notes

- Source signal: 95+ raw `housekeep.py --apply` invocations in transcripts.
- Related memory: `feedback_overview_regen_in_status_commits.md` exists to
  fix the symptom that this skill addresses at the source.
