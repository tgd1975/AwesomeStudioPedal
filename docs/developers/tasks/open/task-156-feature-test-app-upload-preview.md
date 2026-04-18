---
id: TASK-156
title: Feature Test — App upload screen & JSON preview
status: open
opened: 2026-04-19
effort: Small (2-4h)
complexity: Low
human-in-loop: Main
group: feature_test
---

## Description

Execute the functional and usability tests for the Upload screen and JSON Preview screen
as defined in `docs/developers/FEATURE_TEST_PLAN.md` Parts 2.6 and 2.7.

Tests UP-01 and UP-05/UP-06 can be run on the emulator.
Tests UP-03/UP-04/UP-07/UP-08 require a physical device and a connected pedal.

## Pre-conditions

- Flutter app running on physical device (Pixel 9 or similar)
- ESP32 pedal flashed with current firmware, powered on
- BLE connection established before starting UP-02 onwards
- For UP-05: a profiles.json with a missing required field (to force invalid state)

## Tests to execute

### Upload Screen

| Test ID | Description |
|---------|-------------|
| UP-01 | Open upload screen while not connected → Upload button disabled; message shown |
| UP-02 | Connect to pedal, open upload screen → Upload enabled; validation status visible |
| UP-03 | With valid profiles, tap "Upload Profiles" → progress bar; success shown |
| UP-04 | After UP-03: press button A in text editor → correct keypress fires |
| UP-05 | Make profiles invalid; open upload → Upload button disabled; red validation card |
| UP-06 | Tap error card → error details dialog |
| UP-07 | Disconnect pedal mid-upload → error dialog; no crash |
| UP-08 | Tap "Upload Hardware Config" → progress; success; pedal reboots |
| UP-09 | Tap "View JSON Preview" → preview opens with current profiles |
| UP-U1 | Usability: after editing profiles, can tester find "upload to pedal" without instructions? |

### JSON Preview Screen

| Test ID | Description |
|---------|-------------|
| JP-01 | Open JSON preview → monospace, readable JSON displayed |
| JP-02 | Tap "Copy to clipboard" → clipboard contains JSON (verify by pasting) |
| JP-03 | Tap "Share" → system share sheet opens with JSON content |

## Acceptance Criteria

- [ ] All UP-01–UP-09 and JP-01–JP-03 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] UP-U1 usability finding documented
- [ ] Any failures filed as child tasks using the defect template
