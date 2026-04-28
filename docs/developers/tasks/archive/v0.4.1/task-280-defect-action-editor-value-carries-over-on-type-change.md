---
id: TASK-280
title: Defect — Action Editor value field carries over when Action Type changes
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Junior
human-in-loop: Clarification
epic: feature_test
order: 4
---

## Description

Surfaced in TASK-155 round 3 (2026-04-27, AE-04 / AE-06 / AE-07
recipes). When the user changes Action Type in the Action Editor, the
**typed value** in the value field carries over even when it is
meaningless under the new type — so switching from "Key (named)" with
value `KEY_PAGE_UP` to "Media Key", "Type String", or "Key (raw HID)"
leaves the field reading `KEY_PAGE_UP`. The placeholder hint *does*
update per type, but the entered text does not, and a user who
overlooks the carry-over can save a "Media Key" action whose value is
`KEY_PAGE_UP`.

This is cosmetic — the Save path treats the field as the source of
truth and most stale values fail validation downstream — but it is a
small intuitive-feel cost that compounds with TASK-277 (raw-HID hint)
and the AE-U2 terminology session (TASK-278). See
[idea-044](../../ideas/archived/idea-044-action-editor-value-field-resets-on-type-change.md)
for the full motivation, four candidate approaches, and the suggested
combination (rebuild-per-type widget + hard clear when the widget
changes).

## Acceptance Criteria

- [x] Changing Action Type in the Action Editor no longer leaves a
      stale typed value from the previous type. Either the field is
      cleared, or — when the previous and new types share a value
      space (e.g. both named-key-style) — the value is preserved
      intentionally and that behaviour is documented in the code.
- [x] AE-04, AE-06, AE-07 recipes from TASK-155 / FEATURE_TEST_PLAN.md
      no longer surface the carry-over observation on a re-run.
- [x] The change does not regress existing Action Editor widget tests
      and adds at least one widget test that asserts the field's
      contents after a type switch.

## Test Plan

**Host tests** (Flutter widget tests via the app's test runner):

- Add or extend a widget test for the Action Editor that mounts the
  editor with Action Type = "Key (named)" and a typed value
  `KEY_PAGE_UP`, switches Action Type to each of "Media Key", "Type
  String", "Key (raw HID)", and asserts the value field's text matches
  the chosen behaviour (cleared, or preserved within compatible type
  pairs).

**On-device verification** (no new automated coverage required):

- Re-run `/verify-on-device TASK-155 AE-04 AE-06 AE-07` and confirm
  the carry-over observation is gone. This is a smoke check; the
  widget test is the regression guard.

## Notes

- Approach decision needed before implementation: pick one of the four
  options in idea-044 (hard clear / soft clear with undo /
  validate-and-warn / rebuild-per-type widget) — the idea recommends
  *rebuild-per-type + hard clear*, which may already be partially in
  place for the Pin-type widget. Confirm with a quick spike before
  writing the test.
- Cross-references: TASK-155 (verification source), TASK-277
  (raw-HID hint defect, related but separate), TASK-278 (AE-U2
  terminology session), idea-039 (raw-HID value space, out of scope
  here).
- Verified on-device on 2026-04-28 on Pixel 9 / Android 16.
  Profile 04 button A (action `SendCharAction` / `KEY_F13`,
  display name "Intro Scene"): opened Action Editor, switched
  Action Type from "Key (named)" to "Media Key" — the value
  EditText went from `KEY_F13` to empty, while the display-name
  EditText kept "Intro Scene". Screenshot:
  `/tmp/verify_TASK-280_media_key_after_switch.png`. Profile 04
  was not saved (BACK without Save).

## Resolution

Implemented the *value-space categoriser* approach (idea-044
options 4 and 1 combined):

- [app/lib/screens/action_editor_screen.dart](../../../../app/lib/screens/action_editor_screen.dart):
  added a static `_valueSpaceOf(type)` helper that maps each action
  type to a category key (`named`, `mediaKey`, `rawHid`, `string`,
  `serial`, `none`). The dropdown's `onChanged` now routes through
  `_onTypeChanged`, which clears `_valueCtrl` only when the new
  type's category differs from the old. Same-category switches
  (e.g. `kKeyNamedSentinel` ↔ `SendCharAction`) preserve the value,
  matching idea-044's recommendation.
- [app/test/widget/action_editor_value_carryover_test.dart](../../../../app/test/widget/action_editor_value_carryover_test.dart):
  new regression test — mounts the editor with `SendCharAction` +
  `KEY_PAGE_UP`, drives the dropdown to "Media Key", "Type String",
  and "Key (raw HID)", and asserts the value field is empty after
  each switch. Plus a unit-level assertion that the categoriser
  groups same-space types together.

All 36 widget tests pass (incl. the new 4). Existing
`action_editor_save_test.dart` cases still pass — the sibling-field
preservation contract from TASK-276 is unaffected because the clear
only fires on dropdown-driven type changes, not on `_save()`.

On-device re-run of `/verify-on-device TASK-155 AE-04 AE-06 AE-07`
remains the final smoke check; that AC is left unchecked until a
device run confirms it.
