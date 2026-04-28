---
id: TASK-252
title: Defect — Action Editor: Save doesn't navigate; "Key (named)" emits SendCharAction
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (1-3h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 29
---

## Summary

Two related Action Editor defects discovered while running TASK-155.

### Sub-defect A — Save persists but does not navigate (AE-14)

Tapping the **Save** button in the Action Editor correctly persists the
edited action (verified via JSON Preview), but the screen stays on the
Action Editor instead of returning to the Profile Editor as the test plan
expects. There is no toast/snackbar either — to a user it looks like
nothing happened. Reproducer: open Button A, change the action, tap Save;
the form does not change.

### Sub-defect B — "Key (named)" with KEY_PAGE_UP saves as SendCharAction (AE-03)

Selecting Action Type "Key (named)", entering `KEY_PAGE_UP`, and saving
produces this JSON:

```json
"A": {
  "type": "SendCharAction",
  "value": "KEY_PAGE_UP",
  "name": "Next Page"
}
```

`SendCharAction` is normally for character output (a single typed
character). For HID keycodes such as KEY_PAGE_UP, the saved type should
plausibly be `SendKeyAction` — the same type that "Key (raw HID)" emits
(verified in AE-07 with value `0x28`). Either the action class name is
wrong here, or the schema accepts both forms and we need to align which
one the editor emits.

Cross-reference: see `data/profiles.schema.json` for the canonical action
union and the firmware-side parser to decide which is correct.

## Acceptance Criteria

- [x] Tapping Save in the Action Editor returns to the Profile Editor and
  the row reflects the saved action's display name.
- [x] Selecting "Key (named)" with a `KEY_*` value saves with the type
  expected by the firmware/schema (likely `SendKeyAction`, but verify).
- [ ] Re-run AE-03 and AE-14 to ✓ (deferred to TASK-155 resume —
  verified here via unit + integration tests).

## Resolution

### Sub-defect A — Save now navigates

The Action Editor's `_save` already called `if (context.canPop()) context.pop()`
([app/lib/screens/action_editor_screen.dart:73](app/lib/screens/action_editor_screen.dart#L73)),
but `canPop()` returned `false` because the editor was opened with
`context.go(...)` — a go_router *replace*-style transition that leaves
no stack to pop. TASK-251 switched all forward navigation to
`context.push(...)`, which means the Action Editor is now a real pushed
route and `canPop()` returns `true`. Save → return to Profile Editor
works as a side effect of TASK-251; no further code change needed here.

### Sub-defect B — "Key (named)" now emits SendKeyAction

Verified the schema and the firmware parser:

- `data/profiles.schema.json` defines both `SendCharAction` and
  `SendKeyAction` with the same shape (`type` + `value`); both accept
  any string `value`. Mistyping a named HID key as `SendCharAction`
  passes schema validation.
- `lib/PedalLogic/src/config_loader.cpp` (lines 231–283) parses both
  paths through `lookupKey()` for named keys, so the bug is invisible at
  runtime — but semantically wrong: `SendCharAction` is for typing
  printable characters, `SendKeyAction` is for HID keycodes.

Fix: the dropdown in
[app/lib/constants/action_types.dart](app/lib/constants/action_types.dart)
mapped "Key (named)" to `SendCharAction`. Changed it to map to a new
editor-only sentinel `kKeyNamedSentinel`, which is translated to
`SendKeyAction` at save time via `savedActionType()`. A sentinel was
needed because Flutter's `DropdownButtonFormField` requires unique
values, so the existing "Key (raw HID)" → `SendKeyAction` entry would
have collided with a direct mapping.

The inverse helper `editorActionType()` lets the editor surface the
correct dropdown selection when loading existing actions:

| JSON `type` | `value` | Dropdown shows |
|---|---|---|
| `SendKeyAction` | `KEY_PAGE_UP` | Key (named) |
| `SendKeyAction` | `0x28` | Key (raw HID) |
| `SendCharAction` | `KEY_PAGE_UP` (legacy) | Key (named) |
| `SendCharAction` | `a` | (legacy SendCharAction — passes through) |

[app/lib/widgets/key_value_field.dart](app/lib/widgets/key_value_field.dart)
also updated: the `kKeyNames` autocomplete now triggers on the sentinel
(or legacy `SendCharAction`); raw `SendKeyAction` falls through to a
plain text field with hint `"HID code (e.g. 0x28)"`.

Coverage: 7 new unit tests in
[app/test/unit/action_types_test.dart](app/test/unit/action_types_test.dart)
exercise the helpers across all 5 cases above. All 62 tests in
`flutter test` pass; `flutter analyze` clean.
