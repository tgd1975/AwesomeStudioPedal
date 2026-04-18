---
id: TASK-155
title: Feature Test — App action editor (all action types)
status: open
opened: 2026-04-19
effort: Small (2-4h)
complexity: Low
human-in-loop: Main
group: feature_test
---

## Description

Execute the functional and usability tests for the Action Editor screen covering all supported
action types, as defined in `docs/developers/FEATURE_TEST_PLAN.md` Part 2.5.

All tests can be performed on the emulator; use JSON Preview to verify produced JSON structure
without needing a live pedal.

## Pre-conditions

- Flutter app running on test device or Lean_API33 emulator
- At least one profile with at least one button slot accessible

## Tests to execute

### Named Key & Media Key

| Test ID | Description |
|---------|-------------|
| AE-01 | Select "Key (named)" → value field with autocomplete appears |
| AE-02 | Type "KEY_F" → autocomplete shows KEY_F1, KEY_F2 … KEY_F12 etc. |
| AE-03 | Select KEY_PAGE_UP, display name "Next Page", Save → row shows "Next Page"; JSON correct |
| AE-04 | Select "Media Key" → autocomplete; type "PLAY" filters results |
| AE-05 | Select MEDIA_PLAY_PAUSE, Save → JSON shows `SendMediaKeyAction` / `MEDIA_PLAY_PAUSE` |

### String & Raw Key

| Test ID | Description |
|---------|-------------|
| AE-06 | Select "Type String", enter "ctrl+z" → JSON shows `SendStringAction` |
| AE-07 | Select "Key (raw HID)", enter "0x28" → JSON shows `SendKeyAction` / `0x28` |

### Pin Actions

| Test ID | Description |
|---------|-------------|
| AE-08 | Select "Pin High" → pin number field appears |
| AE-09 | Enter pin 27, Save → JSON shows `PinHighAction` / `27` |
| AE-10 | Enter non-numeric pin value → field shows error; Save blocked |
| AE-11 | Select "Pin High While Pressed", pin 14, Save → JSON shows `PinHighWhilePressedAction` |

### Serial Output

| Test ID | Description |
|---------|-------------|
| AE-12 | Select "Serial Output", enter "test message" → JSON shows `SerialOutputAction` |

### Save / Cancel

| Test ID | Description |
|---------|-------------|
| AE-13 | Tap Save with no action type selected → Save disabled or shows error |
| AE-14 | Tap Save with action type set → returns to Profile Editor; row updates |
| AE-15 | Navigate back without tapping Save → action unchanged |

### Usability

| Test ID | Description |
|---------|-------------|
| AE-U1 | Musician sets button A to "Play/Pause media" without guidance; target < 60 s |
| AE-U2 | Is "Key (named)" vs "Key (raw HID)" vs "Media Key" terminology clear to a non-developer? |

## Acceptance Criteria

- [ ] All AE-01–AE-15 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] AE-U1 timed and result noted; AE-U2 findings documented
- [ ] Any failures filed as child tasks using the defect template
