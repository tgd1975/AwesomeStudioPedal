---
id: TASK-276
title: Defect — Action Editor drops `longPress` field when an action is re-saved
status: closed
closed: 2026-04-27
opened: 2026-04-27
effort: Small (2-4h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Verify
epic: feature_test
order: 42
---

## Description

Surfaced while running TASK-155 round 3 on 2026-04-27 (Pixel 9, Android 16,
verified via `/verify-on-device TASK-155 AE-03`).

The starter `data/profiles.json` ships profile `01 Score Navigator`
with Button A having both a primary action and a `longPress`:

```json
"A": {
  "type": "SendCharAction",
  "value": "KEY_PAGE_UP",
  "name": "Prev Page",
  "longPress": {
    "type": "SendCharAction",
    "value": "KEY_HOME",
    "name": "First Page"
  }
}
```

When the AE-03 recipe opened Button A in the Action Editor, edited the
display name, and tapped Save, the new JSON contained **only** the
primary action — the `longPress` block was dropped:

```json
"A": {
  "type": "SendKeyAction",
  "value": "KEY_PAGE_UP",
  "name": "Next Page"
}
```

The Action Editor UI does not expose `longPress` at all (the Action
Editor for Button A only edits the primary action), so the user has no
warning that re-saving an action will silently discard a configured
long-press behaviour. This is silent data loss in the round-trip
load → edit-primary → save.

## Reproducer

1. On Pixel 9 (or any device), import `data/profiles.json` into the
   app so profile `01 Score Navigator` is loaded.
2. Open Profile List → JSON Preview, confirm Button A has both a
   primary `SendCharAction` and a `longPress` sub-object.
3. Open profile 01 → tap Button A → in the Action Editor change only
   the display name (or any unrelated field), tap Save.
4. Open Profile List → JSON Preview, observe Button A no longer has
   the `longPress` field.

## Acceptance Criteria

- [ ] Re-saving an action through the Action Editor preserves any
      pre-existing `longPress` block on the same button (round-trip
      load → save → reload is byte-equivalent for fields the UI does
      not edit).
- [ ] A host unit test under `app/test/` covers the round-trip:
      load profile JSON with `longPress`, simulate a primary-action
      edit + save, assert the resulting JSON still contains the
      original `longPress`.
- [ ] FEATURE_TEST_PLAN.md AE-03 round-3 note's reference to TASK-276
      can be marked resolved on the next TASK-155 round (if any).

## Test Plan

**Host tests** (`make test-host`):

- Add or extend a Flutter widget/unit test in `app/test/` that
  constructs a `Profile` with a `longPress` on Button A, runs it
  through the Action Editor's save path (or its underlying serializer),
  and asserts the serialised JSON still contains `longPress`.

**On-device verification** via `/verify-on-device TASK-276 …` once a
recipe is added (the TASK-155 AE-03 recipe already exercises this
exact code path; a small assertion extension would suffice).

## Notes

- Surfaced in TASK-155 round 3, 2026-04-27. Screenshot:
  `/tmp/verify_TASK-155_ae_03_json_preview.png`.
- Likely root cause: the Action Editor's Save path constructs a fresh
  action object from form state and replaces the button's value
  outright, instead of merging with the existing button object's
  unedited fields. The fix is a merge rather than a replace, scoped
  to fields the editor does not own (`longPress`, and any future
  similarly-managed sibling fields).
- Cross-reference: TASK-252 (the Save-navigation + SendKeyAction fix
  that re-enabled the round-trip in the first place) — closed
  2026-04-26.
- Worth checking whether the same bug exists for any other
  sibling-of-primary fields the schema supports (e.g. button-level
  metadata) — same merge-vs-replace pattern.
