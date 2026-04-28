---
id: TASK-270
title: effort_actual on close — post-hoc t-shirt size written by /ts-task-done
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Clarification
epic: task-system-rollout
order: 3
prerequisites: [TASK-268, TASK-269]
---

## Description

Have `/ts-task-done` write `effort_actual` to the task frontmatter
immediately after `effort:` — a post-hoc t-shirt-size estimate of how big
the work actually turned out to be. Both `effort` (original estimate, never
edited after creation) and `effort_actual` stay forever — the comparison
is the point, and is the data source for the burn-up chart's actual-hours
line in TASK-271.

Lands after TASK-269 so the canonical effort vocabulary is settled before
this task references it.

Canonical t-shirt sizes (from IDEA-043 §4 — the six-size set wins; the
five-size set in the original sub-idea is replaced):

| Size | Label |
|---|---|
| XS  | `XS (<30m)` |
| S   | `Small (<2h)` |
| M   | `Medium (2-8h)` |
| L   | `Large (8-24h)` |
| XL  | `Extra Large (24-40h)` |
| XXL | `XXL (>40h)` |

`/ts-task-new`, `/ts-task-done`, `/ts-epic-new`, and `/ts-task-active` (if
it ever writes effort, which it doesn't today) all write only these strings.
Legacy variants in already-closed tasks are read with normalization (handled
in TASK-271's `LEGACY_MIDPOINTS`) but never written.

`/ts-task-done` gets a new step **2a** — between current steps 2 and 3:

> **2a.** Determine `effort_actual` and write it to frontmatter immediately
> after `effort:`. If the user passed `--effort-actual SIZE`, use that.
> Otherwise pick a t-shirt size from the work product: diff size, files
> touched, commits between `opened:` and today, pause/blocker history.
>
> **No-peek rule:** when judging from the work product, do **not** read the
> original `effort:` value first. Pick the actual, then reveal the original.
> The same agent estimating both ends has a regression-toward-"about right"
> bias; the chart only earns its keep if the gap is honest.

The no-peek rule is a behavioral nudge sequenced into the skill body — read
only the body and diff first, write `effort_actual`, *then* read the rest of
frontmatter. Procedural, not enforced (per §4 option 2 recommendation).

All edits land **once** in `awesome-task-system/` (per TASK-268), then
synced to live.

## Acceptance Criteria

- [x] `/ts-task-done` writes `effort_actual` to frontmatter immediately after `effort:`.
- [x] `--effort-actual SIZE` flag accepted on `/ts-task-done`; `SIZE` matches the canonical six labels.
- [x] No-peek sequencing is documented in the skill body (read body + diff → write `effort_actual` → then read original `effort:`).
- [x] `/ts-task-new` `--effort` accepts only the canonical six labels; prose references the canonical vocabulary table; documents that legacy values are tolerated on read but never written.
- [x] `/ts-epic-new` `--effort` follows the same canonical-six update.
- [x] Legacy labels in already-closed tasks remain readable round-trip (housekeep, OVERVIEW generation, future burn-up parsing all tolerate them).
- [x] All changes synced from `awesome-task-system/` to live; divergence guard clean.

## Test Plan

**Host tests** (`make test-host` and the existing `scripts/tests/test_housekeep.py` runner, mirrored into the package per TASK-268):

- Add cases for the canonical-six vocabulary:
  - housekeep / overview render correctly with each of the six labels
  - legacy labels (`Trivial (<30m)`, `Small (1-2h)`, `Small (1-3h)`, `Small (2-4h)`, `Large (>8h)`) still parse and render
- Skill behavior verified manually against a real close (the `effort_actual` write is a one-line skill change, not script logic).

No on-device tests — change is pure tooling.

## Prerequisites

- **TASK-268** — single source of truth, so the skill edits land once.
- **TASK-269** — settles the canonical vocabulary references before this task writes them; sequenced for tidy commits (per §8 Q5 recommendation).

## Non-goals

- No retroactive backfill on already-closed tasks.
- No re-fill on later passes — if `effort_actual` stays blank, it stays blank forever.
- No replacing `effort:` with `effort_actual`. Both stay forever.

## Notes

- Effort **S (<2h)**. One skill edit (well-scoped), two minor skill updates for vocabulary, no script changes, no index changes.
- The no-peek rule is honor-system as written. Sequencing it in the skill is a behavioral nudge, not a guarantee — note this explicitly in the skill body.
- Burn-up chart's `LEGACY_MIDPOINTS` table (TASK-271) is the only place legacy labels need explicit mapping. Other readers (housekeep, overview) just display whatever string is in the frontmatter.
