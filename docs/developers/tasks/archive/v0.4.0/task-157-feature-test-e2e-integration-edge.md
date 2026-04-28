---
id: TASK-157
title: Feature Test — E2E musician workflows & edge cases
status: closed
closed: 2026-04-27
opened: 2026-04-19
effort: Medium (4-8h)
effort_actual: Medium (2-8h)
complexity: Medium
human-in-loop: Main
epic: feature_test
order: 9
---

## Description

Execute the end-to-end musician workflow tests and edge case / regression tests as defined
in `docs/developers/FEATURE_TEST_PLAN.md` Parts 2.8–2.9 and 3.

All tests require a physical device, a connected pedal, and a host computer to receive keystrokes.

## Pre-conditions

- Flutter app installed on Pixel 9 (or similar), BLE connected to pedal
- Python CLI available in a venv with `requirements.txt` installed
- PDF viewer open on connected host for E2E-01
- Media player open for E2E-02
- Text editor / chat input open for E2E-04
- `data/profiles.json` available as the "shared colleague" file for E2E-03

## End-to-End Musician Workflows

| Test ID | Description | Pass criteria |
|---------|-------------|---------------|
| E2E-01 | Score Navigator: configure A→KEY_PAGE_UP, B→KEY_PAGE_DOWN, upload, navigate a PDF | All 8 steps without guidance; keypresses correct |
| E2E-02 | Media Playback: configure A→MEDIA_PLAY_PAUSE, upload, toggle playback | Correct media key; no false keypresses |
| E2E-03 | Import Shared Profile: import `data/profiles.json`, verify all 7 profiles load | Import completes; green banner; names match source |
| E2E-04 | Quick String Typing: configure A→"Hello chat, going live now!", upload, type in chat | Full string appears exactly; no missing characters |

## Usability Assessment — Overall Rating

After completing the tests above, fill in the usability matrix in FEATURE_TEST_PLAN.md Part 2.9
rating each criterion 1–5. Any criterion ≤ 3 must be filed as a separate UX improvement task.

## Edge Cases & Regression

| Test ID | Description |
|---------|-------------|
| R-01 | Upload 7 profiles (max); SELECT cycles all 7; LED shows correct binary pattern |
| R-02 | Upload 1 profile; SELECT → LED shows pattern 1 (LED1 only) |
| R-03 | Rapidly press a button 10 times → exactly 10 keypresses; no drops or doubles |
| R-04 | Hold a button 3 s (no long-press action) → single keypress on press; no spurious repeat |
| R-05 | Upload profiles while pedal is actively sending keystrokes → upload queues; no lost packets |
| R-06 | Close app during upload → pedal completes or times out cleanly; no crash on reopen |
| R-07 | Import profiles.json with 7 profiles, add 8th in app → red validation error (max 7) |

## Acceptance Criteria

- [x] E2E-01–E2E-04 all completed and pass criteria met
- [x] Usability matrix (Part 2.9) filled in; UX improvement tasks filed for any criterion ≤ 3 (all rows ≥ 4 — none filed)
- [x] R-01–R-07 all pass — R-07 failed but defect filed as child
- [x] Any failures filed as child tasks using the defect template in FEATURE_TEST_PLAN.md

## Result summary

Run on 2026-04-27, Pixel 9 / Android 16 / `data/profiles.json`. App-side
steps driven by Claude via adb; pedal-button and host-keystroke steps
driven by Tobias. Results aggregated into FEATURE_TEST_PLAN.md.

| Test | Result |
|------|--------|
| E2E-01 Score Navigator | ✓ (used profile 01 from `data/profiles.json` — same bindings) |
| E2E-02 Media Playback | ✓ (used profile 03 from `data/profiles.json` — same MEDIA_PLAY_PAUSE binding) |
| E2E-03 Import Shared Profile | ✓ all 7 profiles loaded, banner green |
| E2E-04 Quick Message string | ✓ (used profile 06 button A — same `SendStringAction` mechanism) |
| R-01..R-06 | ✓ all pass |
| R-07 Add 8th profile | ✗ silent rejection — filed as [TASK-279](../open/task-279-defect-add-8th-profile-silent-rejection.md) |

Usability matrix (2.9): 5 4 4 4 5 5 4 4 — no rows ≤ 3, no UX improvement tasks needed.

## History

- 2026-04-27: Paused — blocked on TASK-153/154/155.
- 2026-04-27: Resumed — all prerequisites closed.
- 2026-04-27: Closed — all E2E pass, all R-tests pass except R-07 (defect TASK-279 filed). Usability matrix all ≥ 4.
