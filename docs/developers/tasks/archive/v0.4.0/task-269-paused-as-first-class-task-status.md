---
id: TASK-269
title: Paused as first-class task status — paused/ folder, status, and lifecycle
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Large (8-24h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Clarification
epic: task-system-rollout
order: 2
prerequisites: [TASK-268]
---

## Description

Promote `paused` to a first-class task status alongside `open`, `active`, and
`closed`. Today, paused tasks live in `active/` with a `## Paused` body
section — they inflate active counts and can't be filtered out cleanly by
downstream tooling (notably the burn-up chart in TASK-271, which needs
"anything not closed since the tag" to exclude paused work).

After this lands, paused tasks live in `docs/developers/tasks/paused/`, carry
`status: paused`, and the lifecycle skills (`ts-task-pause`, `ts-task-active`,
`ts-task-done`, `ts-task-list`) move them between folders correctly.

Lifecycle (from IDEA-043 §3):

```
open ──activate──> active ──pause──> paused
                      │                │
                      │                └── continue ──> active
                      │                │
                      │                └── done ──> closed
                      │
                      └── done ──> closed

closed ──reopen──> open  (only when explicitly demanded)

active/paused ──> open  (escape hatch — only on explicit user request,
                         e.g. "I activated this by mistake"; manual git mv)
```

Configuration truth-table (paused depends on active):

| `active.enabled` | `paused.enabled` | Effective `paused` |
|---|---|---|
| `true`  | `true`  | enabled |
| `true`  | `false` | disabled |
| `false` | `true`  | **disabled** (forced off — active is gone) |
| `false` | `false` | disabled |

All edits land **once** in `awesome-task-system/` (per TASK-268), then
`scripts/sync_task_system.py` propagates to live.

Bundles **§9.1 of IDEA-043** — `EPICS.md` per-epic task tables sort
deterministically: primary by status (`open` > `paused` > `active` > `closed`,
open at the top), secondary by `order:` ascending. Lands here because the
sort references `paused` and this task already touches the same renderer
(`update_task_overview.py`).

## Acceptance Criteria

- [x] `docs/developers/tasks/paused/` folder exists; `housekeep.py` scans it.
- [x] `/ts-task-pause` moves the file to `paused/` and sets `status: paused`. The `## Paused` body section becomes optional documentation (status is authoritative).
- [x] `/ts-task-active` adds the `paused → active` transition (flip status, `git mv paused/ → active/`); existing `open → active` path unchanged.
- [x] `/ts-task-done` can close from `paused` directly without an intermediate transition.
- [x] `/ts-task-list` default-hides paused; `--paused` and `--all` flags expose them.
- [x] `/tasks` skill default-hides paused (matches the "what should I work on?" intent), with a flag for explicit inclusion.
- [x] `OVERVIEW.md` renders a "Paused Tasks" section between Active and Open. Counts header gains `Paused: N`. Per-epic progress-bar denominator includes paused.
- [x] `KANBAN.md` renders a Paused lane between Active and Closed.
- [x] `EPICS.md` renders the per-epic paused count alongside active/open/closed (count only, not propagation — per §8 Q2 recommendation).
- [x] `EPICS.md` per-epic task tables sort by status (`open` > `paused` > `active` > `closed`) then by `order:` ascending. (Bundles §9.1.)
- [x] `task_system_config.py` `DEFAULTS["tasks"]` gains `"paused": {"enabled": True}`. `paused_enabled(cfg)` helper returns `active.enabled AND paused.enabled` per the truth table.
- [x] `awesome-task-system/config/task-system.yaml` gains the `paused:` section; opt-out (set `paused.enabled: false`) collapses the status correctly in housekeep and the renderers.
- [x] `release` skill body documents that paused inherits open/active behavior (don't block, don't appear shipped, carry forward).
- [x] `awesome-task-system/TASK_SYSTEM.md` documents `paused` as a status and the escape-hatch (`active`/`paused` → `open` is manual, rare). `awesome-task-system/LAYOUT.md` adds `paused/` to the folder tree.
- [x] All changes synced from `awesome-task-system/` to live; divergence guard clean.

## Test Plan

**Host tests** (`make test-host` and the existing `scripts/tests/test_housekeep.py` runner, mirrored into the package per TASK-268):

- Extend `scripts/tests/test_housekeep.py`:
  - paused folder is scanned and its tasks counted
  - `paused → active` transition moves the file out of `paused/`
  - `paused → closed` (via `ts-task-done`) moves the file to `closed/`
  - paused-disabled config (`paused.enabled: false`) collapses the status — paused tasks render as their pre-pause status
  - active-disabled config forces paused off too (truth-table row 3)
- Add cases for `update_task_overview.py`:
  - paused section renders between Active and Open with the correct count
  - per-epic progress-bar denominator includes paused
  - `EPICS.md` per-epic task tables sort by `(status, order)` deterministically
- Add a case for `KANBAN.md`: Paused lane appears between Active and Closed.
- Skill-behavior tests are exercised by the existing skill harness (no new framework needed).

No on-device tests — change is pure tooling.

## Prerequisites

- **TASK-268** — establishes `awesome-task-system/` as the single source of truth and the sync script. Without it, every change here would have to be applied twice (live + package), and the divergence guard would not catch sync misses.

## Notes

- **Migration of the 4 old-model paused tasks (153/154/155/156) is explicitly skipped** (per §8 Q3). They will continue to count as active until each is touched, at which point they get re-paused under the new mechanics or progress normally. Worst-case overcount: 4 tasks, decaying to zero over weeks. Do not bundle a one-shot migration script into this task.
- **Per-epic paused state in EPICS.md is count-only, not propagation** (per §8 Q2 recommendation in the sub-idea body). An epic with all-paused subtasks is still derived as active or open by housekeep — paused only contributes to the per-epic count.
- **Escape hatch (active/paused → open) is manual.** Document in `TASK_SYSTEM.md` as the exception path; do **not** add a `--to-open` flag to any existing skill.
- **Largest single piece of EPIC-020.** `update_task_overview.py` is the biggest single diff: new `PAUSED_DIR`, paused count, paused badge, paused section, denominator change, `EPICS.md` sort.
- Effort **L (8–24h)**, realistically closer to a full day. With TASK-268 done first, no doubling cost.
