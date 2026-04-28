---
id: TASK-274
title: EPICS.md per-epic sections in index order with back-to-top links
status: closed
closed: 2026-04-27
opened: 2026-04-27
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: No
epic: task-system-rollout
order: 6
---

## Description

`generate_epics_md()` in `housekeep.py` renders the **index** table sorted
numerically by EPIC-NNN id (via `_epic_sort_key`), but the per-epic
**sections** below are emitted in alphabetical order
(`for epic_name in sorted(by_epic.keys())`). The two orders disagree, so
clicking an index link can land the reader well above or below the
expected position and the page does not read top-to-bottom in any
consistent sequence.

Two small fixes:

1. Render the per-epic sections in the same order as the index — sort by
   `_epic_sort_key` (numerical EPIC-NNN id ascending). The Unassigned
   section, if present, stays last to match its position at the bottom
   of the index table.
2. Add a back-to-top affordance under each per-epic heading — a small
   `[↑ back to top](#index)` link so the reader can return to the index
   without scrolling. Same affordance under the Unassigned section.

All edits land **once** in `awesome-task-system/scripts/housekeep.py`
(per TASK-268), then `scripts/sync_task_system.py` propagates to live.

## Acceptance Criteria

- [x] Per-epic sections in `EPICS.md` are emitted in the same order as
      the index table (numerical EPIC-NNN id, ascending). The Unassigned
      section, when present, stays last.
- [x] Each per-epic section heading is followed by a `[↑ back to top](#index)`
      link (or equivalent affordance pointing at the `## Index` anchor).
      The Unassigned section gets the same affordance.
- [x] Generated `EPICS.md` byte-stable on a second `housekeep --apply`
      with no input changes (idempotency unaffected).

## Test Plan

**Host tests** (`scripts/tests/test_housekeep.py`):

- Extend `TestGenerateEpicsMd`:
  - Set up two epics with non-alphabetical EPIC-NNN ids (e.g. EPIC-002 named
    `zebra`, EPIC-010 named `alpha`). Assert that the per-epic sections
    appear in the order EPIC-002 → EPIC-010 in the rendered output, **not**
    alphabetical (alpha → zebra).
  - Assert each per-epic heading is followed by a `[↑ back to top](#index)`
    link.

No on-device tests — change is pure tooling.

## Notes

- Touches only `generate_epics_md()` in `awesome-task-system/scripts/housekeep.py`.
- The fix for (1) is mechanical: replace `for epic_name in sorted(by_epic.keys()):`
  with `for epic_name in sorted(by_epic.keys(), key=_epic_sort_key):` (the
  `_epic_sort_key` closure is already defined in the surrounding scope).
- The fix for (2) is one extra line per section emitter.
- No skill changes, no docs changes, no config changes.
