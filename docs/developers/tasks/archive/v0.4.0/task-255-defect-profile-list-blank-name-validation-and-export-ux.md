---
id: TASK-255
title: Defect — Profile List blank-name validation invisible & Export UX (share sheet + UUID filename)
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (1-3h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 31
---

## Summary

Two related defects found while running TASK-154 (PL-03, PL-10) on the
Profile List screen.

### Sub-defect A — app-PL-DEFECT-01: blank-name validation has no visible error (PL-03)

In the "New Profile" dialog, leaving the Name field blank and tapping Add
correctly keeps the dialog open (validation prevents submission), but **no
inline error message is shown** on the Name field. The user has no
feedback about why nothing happened. Compare with standard Material text
fields where an `errorText` appears below the field on validation failure.

Likely fix: in the dialog's name field, surface a `FormFieldState.errorText`
("Name is required") when the user attempts to submit with an empty value,
or block the Add button while the field is empty.

### Sub-defect B — app-PL-DEFECT-02: Export shares a UUID-named file via share sheet (PL-10)

The PL-10 test plan expects: "File appears in Downloads; is valid JSON
matching current state". In practice, [app/lib/services/file_service.dart:54](app/lib/services/file_service.dart#L54)
`exportProfiles` calls `Share.shareXFiles` with `XFile.fromData`, which:

1. Opens the Android share sheet rather than writing to Downloads.
2. Writes the temp file under
   `cache/<uuid>/<uuid>.json` (observed example:
   `13e-11f1-8711-27deaeeff2c3.json`) — not a friendly name like
   `profiles.json` even though `name: 'profiles.json'` is passed to
   `XFile.fromData`. The share-sheet downstream apps (Files, Drive, etc.)
   pick up the cache filename, not the friendly name.

Functionally the JSON content is correct and valid. The deviation is UX:
users cannot find the exported file in Downloads, and the suggested
filename is not human-readable.

Likely fix options:

- A: Save directly to `getExternalStoragePublicDirectory(DOWNLOADS)`
  (or scoped `MediaStore.Downloads` on Android 11+) using a friendly
  date-stamped filename like `profiles-2026-04-26.json`. Optionally
  *also* offer a Share button.
- B: Keep the share sheet but ensure the temp file's on-disk filename
  matches the friendly name (write to a stable path under cache and
  pass that `File` to `Share.shareXFiles` instead of `XFile.fromData`).

## Tests affected

- TASK-154 PL-03 (sub-defect A).
- TASK-154 PL-10 (sub-defect B).

## Acceptance Criteria

- [x] Submitting "New Profile" with blank name shows a visible inline
  error on the Name field (or the Add button is disabled until a non-empty
  name is entered).
- [x] Export produces a file with a friendly, predictable name
  (`profiles.json` or `profiles-<date>.json`) — either saved to Downloads
  directly or shared via a path that preserves the friendly filename.

## Resolution

### Sub-defect A — visible blank-name validation

The inline `_addProfile` builder in `ProfileListScreen` used a plain
`TextField` with an `if (nameCtrl.text.isNotEmpty)` guard inside the
Add-button handler — submission was blocked but no feedback was
shown. Replaced the inline builder with a private `_NewProfileDialog`
(StatefulWidget) wrapping the fields in a `Form` with
`autovalidateMode: AutovalidateMode.onUserInteraction`. The Name field
is a `TextFormField` with a `validator` that returns
`'Name is required'` for empty input. Tapping Add now triggers
`Form.validate()`, surfacing the message under the Name field.

The dialog accepts the `ProfilesState` via constructor (resolved by the
caller via `context.read`) rather than calling `Provider.of` from inside
`showDialog`'s root-navigator context — that decouples the dialog from
the provider tree and was a self-correcting tweak after the first test
run flagged a `ProviderNotFoundException`.

### Sub-defect B — friendly export filename

[app/lib/services/file_service.dart](app/lib/services/file_service.dart)
previously used `XFile.fromData(...)` which writes the bytes to a tmp
file under a UUID directory; downstream apps in the share sheet picked
up that UUID-shaped filename. Switched to writing the JSON to the system
temporary directory under a date-stamped filename
(`profiles-YYYY-MM-DD.json`) and passing that real `File` path to
`Share.shareXFiles` via `XFile(path, …, name: filename)`. Apps in the
share sheet now see the friendly name.

Picked option B from the task ("keep the share sheet, friendly path")
per the user's preference for the simpler fix — no Downloads-directory
plumbing, no extra storage permissions on Android.

### Coverage

- [app/test/unit/file_service_export_filename_test.dart](app/test/unit/file_service_export_filename_test.dart) —
  3 tests cover the new public `FileService.exportFilenameForDate(now)`
  helper (date format, zero-padding, no UUID-shaped fragment).
- [app/test/widget/profile_list_new_profile_dialog_test.dart](app/test/widget/profile_list_new_profile_dialog_test.dart) —
  2 widget tests: blank-Add surfaces "Name is required"; valid-Add
  closes the dialog and adds the profile.

`flutter test` 66 → 71, all green; `flutter analyze` clean.

The on-device verification of PL-03 and PL-10 happens during TASK-154
resume — both ACs above are covered structurally by the new tests.
