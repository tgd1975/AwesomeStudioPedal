---
id: TASK-265
title: Defect — Action Editor crashes with Flutter dropdown assertion when opened on a profile whose action value cannot be resolved
status: closed
closed: 2026-04-27
opened: 2026-04-26
effort: Small (2-4h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Main
epic: feature_test
order: 36
---

## Description

Discovered while running TASK-154 PE-02 on 2026-04-26.

When the Profile List contains a profile whose action `value` is not
resolvable (e.g. a `SendCharAction` with `value: "NOT_A_VALID_KEY_VALUE"`)
and the user opens the Action Editor for that button, the editor body
turns into the red Flutter error screen with this assertion:

```
package:flutter/src/material/dropdown.dart:1830 pos 10:
'items == null || items.isEmpty || (initialValue == null && value == null) ||
 items.where((DropdownMenuItem<T> item) => item.value == (initialValue ?? value)).length == 1':

There should be exactly one item with [DropdownButton]'s value: SendCharAction.
Either zero or 2 or more items were detected with the same value.
```

The error message names `SendCharAction` — that's the **Action Type**
dropdown's value, which is one of a small finite set. So the bug is
not directly that the user-supplied `value` is invalid; rather, when
the editor reduces the option set based on the current state (perhaps
filtering action types compatible with the parsed value), the result
ends up with zero or two-plus matching items, breaking the dropdown's
invariant.

The app bar still renders ("Button A" title, Save button), but the
body is unusable. The user's only recourse is system Back.

## Reproducer

1. Build `profiles_invalid_action.json` per TASK-264 step 1.
2. In the app: Edit profiles → Import JSON → pick that file.
3. Tap "01 Score Navigator" (or whichever profile holds the bad
   action).
4. Tap the Button A row.
5. Observe: red error screen with `dropdown.dart:1830` assertion.

Reproduced on the second consecutive open in the original session;
re-import + cold open also reproduces. Not an intermittent state
issue.

## Acceptance Criteria

- [ ] Opening the Action Editor on a profile with an unresolvable
      action `value` does not crash. The screen renders normally.
- [ ] The unresolvable value is surfaced inline in the form (e.g.
      the value field shows the literal string with a "this value
      is not a recognised key" hint), so the user can fix it.
- [ ] The Action Type dropdown invariant ("exactly one item per
      value") is preserved regardless of `value` field state. If
      the dropdown's items list is being filtered by the value
      content, the filter must be either (a) removed, or (b)
      adjusted so the current type is always present.
- [ ] No Flutter dropdown assertion in `flutter run` console output
      for the reproducer above.

## Test Plan

**Flutter widget test** (`app/test/`):

- Build the Action Editor widget with a `SendCharAction` whose
  `value` is `"NOT_A_VALID_KEY_VALUE"` and assert the dropdown
  builds without throwing.
- Add a regression test for the same behaviour with each action
  type that has a value field (`SendKeyAction`, `SendMediaKeyAction`).

**Manual on-device:** re-run TASK-154 PE-02 with
`profiles_invalid_action.json` imported and confirm the editor
opens normally.

## Notes

- Closely related to TASK-264. TASK-264 makes the bad value
  *visible* in the Profile List; this task makes the editor
  *survive* the bad value once the user gets there. They will
  likely be fixed together but they are independently verifiable.
- The crash is a hard screen-level failure; categorising it as
  "Small (2-4h)" assumes the dropdown items list is straightforward
  to audit. If there is filtering coupling between Action Type and
  value content, this could grow.
- Surfaced in TASK-154 PE-02 re-run on 2026-04-26.

## Resolution

Fixed in commit on `feature/idea-realizations` (2026-04-27) — bundled
with TASK-264 since both have the same root cause:

- [app/lib/constants/action_types.dart](../../../../app/lib/constants/action_types.dart):
  `editorActionType()` is now bulletproof — its return value is
  guaranteed to exist in `kActionTypes`, so the DropdownButton
  invariant ("exactly one item per value") is never violated.
  Specifically:
  - `SendCharAction` with a non-`KEY_*`, non-single-char value (the
    crash reproducer's case) maps to the named-key sentinel so the
    editor renders normally and the user can correct the value.
  - `SendCharAction` with a single character maps to `''` (none) —
    the dropdown has no `SendCharAction` item; falling through would
    re-introduce the crash. The user re-selects an action type.
  - Any other unrecognised type maps to `''`.
- [app/lib/screens/action_editor_screen.dart](../../../../app/lib/screens/action_editor_screen.dart):
  Save now blocks (with a SnackBar) when the form's value would not
  resolve at runtime, sharing the
  [ActionValueResolver](../../../../app/lib/services/action_value_resolver.dart)
  used by the Profile List banner — closes TASK-264's last AC ("the
  same check blocks Save in the Action Editor for the same input").

Verified on-device on 2026-04-27 on Pixel 9 / Android 16:

1. Imported `profiles_invalid_action.json` (Button A = SendCharAction,
   value = `NOT_A_VALID_KEY_VALUE`).
2. Tapped Profile 01 → tapped the Button A row. Action Editor opened
   normally — title "Button A", Save button enabled, Action Type
   "Key (named)", value field showing `NOT_A_VALID_KEY_VALUE`,
   display name `Prev Page`. **No red error screen, no
   `dropdown.dart:1830` assertion** in `flutter run` console output.
3. Tapping Save with the bad value still in the field was blocked
   (SnackBar; navigation stayed on Action Editor). Replacing the
   value with `KEY_PAGE_UP` allowed Save and the editor returned to
   Profile Editor with Button A now showing `SendKeyAction /
   Prev Page`.

Screenshots: `/tmp/verify_TASK-155_ae_265_no_crash.png`,
`/tmp/verify_TASK-155_ae_save_blocked.png`.
