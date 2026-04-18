---
id: TASK-154
title: Feature Test — App profile list & profile editor
status: open
opened: 2026-04-19
effort: Small (2-4h)
complexity: Low
human-in-loop: Main
group: feature_test
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

- [ ] All PL-01–PL-13 and PE-01–PE-05 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] PL-U1 and PL-U2 usability findings documented
- [ ] Any failures filed as child tasks using the defect template
