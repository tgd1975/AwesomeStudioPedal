---
id: TASK-283
title: Make UI-driving via adb a first-class skill the agent actually uses
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: Medium (2-8h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Clarification
epic: agent-collaboration-skills
order: 1
---

## Description

The transcript scan shows the agent hand-rolled the
`sleep 1 && adb -s <DEV> shell uiautomator dump /sdcard/ui.xml … python3 ui_find.py`
loop **41+ times** across sessions, plus 200+ raw `adb shell input tap/keyevent`
calls. The existing `/verify-on-device` skill was invoked only 4 times in the
same period.

The mismatch is not because UI work is rare — it is because `verify-on-device`
is framed as a verification skill ("drive a feature-test scenario end-to-end"),
so the agent reaches for raw bash whenever the goal is exploratory ("tap
through it once and see what happens") instead of scripted verification.

This task makes UI-driving the default. Either widen `verify-on-device` so it
covers exploratory drives too, or split out a smaller `/ui-dump` skill that is
the canonical wrapper around dump → pull → `ui_find.py` → tap, and update
`verify-on-device` to delegate to it.

## Acceptance Criteria

- [x] Decide between (a) widening `/verify-on-device` or (b) adding a new
      `/ui-dump` skill — document the choice in the task notes.
- [x] The chosen skill makes the dump+pull+ui_find loop a single invocation
      with no hand-written bash chain.
- [x] The skill description is worded so the agent triggers it for any task
      involving Pixel UI interaction, not only feature-test verification.
- [x] CLAUDE.md (or the skill description) names the trigger explicitly:
      "any time you would type `adb shell uiautomator dump`, invoke this
      skill instead."
- [x] The Pixel device serial is read from a single source (env var,
      `.envrc`, or skill config) — not hard-coded into bash commands.
      Across 148 transcripts the hex `4C200DLAQ0056N` was hard-written
      135 times in one session alone; the skill must own this so it
      survives a phone swap.

## Test Plan

No automated tests required — change is to skill metadata and CLAUDE.md
prose. Validate by checking that a fresh session, when asked to tap through
the Pixel app, invokes the skill rather than hand-rolling adb commands.

## Notes

- Source signal: chat `60437f00-b54e-4d94-bac7-4d4eaba9767c` and 41+ raw
  uiautomator dumps in transcript corpus.
- Related existing skill: `verify-on-device` (used 4×).
- This is the highest-leverage item in the epic — it touches the work flow
  Tobias is most often physically present for.

### Design choice: option (b) — add `/ui-dump` rather than widen `/verify-on-device`

Picked option (b). Rationale:

- `/verify-on-device` is structured around a **named scenario catalog**
  (UP-03, AE-03, AE-04, …) with strict pass/fail assertions and a
  result-handling protocol that writes verification notes back to task
  files. Adding "exploratory drive" as a scenario would dilute its
  purpose and force every ad-hoc tap through scenario-shaped scaffolding.
- `/ui-dump` can stay tiny: dump → pull → optional find → optional tap.
  One helper script ([`.claude/skills/ui-dump/ui_dump.sh`](../../../../.claude/skills/ui-dump/ui_dump.sh)),
  three modes, one Bash invocation per call. That makes it cheap to
  reach for and avoids the "but I'd have to set up a scenario" friction
  that pushed us to raw `adb shell uiautomator dump` 41+ times.
- `/verify-on-device` already has its primitives inline in its SKILL.md
  for self-containment of scenarios — leave them there. CLAUDE.md now
  names both skills with their respective triggers (exploratory →
  `/ui-dump`, scripted scenario → `/verify-on-device`).
- Single source of truth for the device serial: `ANDROID_SERIAL` env var.
  `/ui-dump` reads it once via `resolve_serial`, falling back to a single
  `adb devices` lookup. No hex hard-coded anywhere; phone swap costs only
  re-exporting the env var.
