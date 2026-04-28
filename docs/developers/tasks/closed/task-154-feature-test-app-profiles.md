---
id: TASK-154
title: Feature Test — App profile list & profile editor
status: closed
closed: 2026-04-27
opened: 2026-04-19
effort: Small (2-4h)
effort_actual: Medium (2-8h)
complexity: Low
human-in-loop: Main
epic: feature_test
order: 6
---

## Description

Execute the functional and usability tests for the Profile List and Profile Editor screens
as defined in `docs/developers/FEATURE_TEST_PLAN.md` Parts 2.3 and 2.4.

Most tests can be performed on the emulator; Import/Export (PL-07, PL-10) require a physical
device or emulator with file system access.

## Pre-conditions

- Flutter app running on test device or Lean_API33 emulator
- `data/profiles.json` available to share to the device (e.g. via `adb push` or email)
- For PL-09: a JSON file with valid syntax but an invalid schema (missing `buttons` in a profile)

## Tests to execute

| Test ID | Description |
|---------|-------------|
| PL-01 | No profiles → empty list + visible (+) button; no crash |
| PL-02 | Tap (+) → dialog with name (required) and description (optional) |
| PL-03 | Save with blank name → validation error; dialog stays open |
| PL-04 | Add "My Test Profile" → appears in list |
| PL-05 | Add 3 profiles; drag to reorder → order persists |
| PL-06 | Tap trash icon → profile removed |
| PL-07 | Import valid profiles.json → all profiles load; green validation banner |
| PL-08 | Import invalid JSON (e.g. truncated) → error shown; existing profiles not lost |
| PL-09 | Import JSON that parses but fails schema → error with schema details; profiles not lost |
| PL-10 | Export to Downloads → file appears; is valid JSON matching current state |
| PL-11 | Tap "JSON Preview" → JSON preview opens |
| PL-12 | Profile with invalid action in list → red validation banner "N error(s)" |
| PL-13 | Tap red banner → error details dialog |
| PL-U1 | Usability: can tester find Export to share profiles with a friend without guidance? |
| PL-U2 | Usability: is the difference between Import (replace all) and Add Profile (add one) clear? |
| PE-01 | Tap a profile → editor opens; shows buttons A, B, C, D |
| PE-02 | Tap button A row → action editor opens for slot A |
| PE-03 | Return without saving → original action unchanged |
| PE-04 | Named action displays its display name in the row, not the raw type |
| PE-05 | Button with no action → row shows "(none)" |

## Acceptance Criteria

- [x] All PL-01–PL-13 and PE-01–PE-05 functional tests executed and results recorded in FEATURE_TEST_PLAN.md (PL-05 drag-to-reorder deferred to manual on-device sitting)
- [ ] PL-U1 and PL-U2 usability findings documented — **deferred to a non-developer tester sitting; not a blocker for functional acceptance**
- [x] Any failures filed as child tasks using the defect template (TASK-264, TASK-265 — both closed)

## Progress so far (2026-04-26 — paused mid-run)

Driven via adb on Pixel 9 (4C200DLAQ0056N), same setup as TASK-153/155/156.

**Fixtures prepared and pushed to `/sdcard/Download/` on the device:**

- `profiles_valid.json` — copy of `data/profiles.json` (7 profiles)
- `profiles_invalid_json.json` — truncated mid-value, fails JSON parse
- `profiles_invalid_schema.json` — parses, but first profile lacks required `buttons` field
- Local copies kept in `/tmp/task154-fixtures/` along with all screenshots

**Results so far (not yet written to FEATURE_TEST_PLAN.md):**

| Test | Result | Notes |
|------|--------|-------|
| PL-01 | ✓ | Empty list shows "No profiles yet. Tap + to add one." Green Valid ✓ banner. FAB visible. |
| PL-02 | ✓ | "New Profile" dialog with Name + Description fields, Cancel/Add buttons. |
| PL-03 | ~ | Dialog stays open on blank-name Add — correct — but no visible inline validation error on the Name field. Minor UX defect candidate. |
| PL-04 | ✓ | "My Test Profile" appears in list with trash + reorder handle. |
| PL-05 | deferred | Drag-to-reorder cannot be reliably driven via `adb input swipe` on a Flutter ReorderableListView (needs long-press grab). Verify manually. |
| PL-06 | ✓ | Trash icon removes profile immediately. No confirm dialog (note: the test plan does not require one, but UX consideration). |
| PL-07 | ✓ | Imported `profiles_valid.json` → all 7 profiles loaded; "Profiles imported" snackbar; green Valid ✓ banner. |
| PL-08 | ✓ | Imported `profiles_invalid_json.json` → "Import Failed: JSON parse error: Unexpected end of input". Existing profiles preserved. |
| PL-09 | ✓ | Imported `profiles_invalid_schema.json` → "Import Failed" with explicit schema error: `/profiles/0: required prop missing: buttons from (name: Schema Violator, …)`. Existing profiles preserved. |
| PL-10 | ~ | Export invokes the Android share sheet (not a direct save to Downloads as the test expects). Auto-generated filename is a UUID-ish `13e-11f1-8711-27deaeeff2c3.json`, not a friendly name like `profiles.json` or a date-based one. Functional but UX deviation. |
| PL-11..13 | not run | TODO |
| PE-01..05 | not run | TODO |
| PL-U1, PL-U2 | deferred | Need a non-developer tester. |

**Side observations / candidate defects to file:**

- FAB (+) and many list-row interactive icons have empty `content-desc` — accessibility regression.
- PL-03: blank-name validation has no visible error message on the Name field.
- PL-10: export should write to Downloads with a friendly filename; currently triggers share sheet with UUID-like filename.

**To resume:**

1. Reset app: `adb shell am force-stop com.example.awesome_studio_pedal && adb shell pm clear com.example.awesome_studio_pedal`. Re-import via PL-07 to get a profile set, then continue with PL-11 (JSON Preview button = top-right `{}` icon at device bounds `[954,184]-[1080,310]`).
2. PL-12/13 need an invalid profile in the list — easiest path: edit `profiles_valid.json` to corrupt one action's `value` field, push it to `/sdcard/Download/`, and import.
3. PE-01..05: tap a profile row → editor opens.
4. Then write all results into `docs/developers/FEATURE_TEST_PLAN.md` in the same style as the previous batch (TASK-153/155/156), file defects for failures, and close the task.

## History

- 2026-04-26: Resumed after TASK-251, TASK-255, TASK-256 closed.
- 2026-04-26 (round 2 paused): Re-run end-to-end. PL-12 → TASK-264
  filed (banner stays green for runtime-unresolvable values).
  PE-02 → TASK-265 filed (Action Editor crashes with dropdown
  assertion on the same broken profile).
- 2026-04-27 (close): Re-verified PL-12, PL-13, PE-01..PE-05 on
  Pixel 9 with TASK-264 + TASK-265 fixes in place. Banner now
  flips red for runtime-unresolvable values, error dialog shows a
  precise JSON-pointer + reason, and the Action Editor opens
  cleanly so the bad value can be repaired. PL-05 / PL-U1 / PL-U2
  remain deferred to a manual / non-developer sitting.
