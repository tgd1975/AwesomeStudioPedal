---
id: TASK-157
title: Feature Test — E2E musician workflows, integration & edge cases
status: open
opened: 2026-04-19
effort: Medium (4-8h)
complexity: Medium
human-in-loop: Main
group: feature_test
---

## Description

Execute the end-to-end musician workflow tests, CLI+app integration tests, and edge case /
regression tests as defined in `docs/developers/FEATURE_TEST_PLAN.md` Parts 2.8–2.9, 3, and 4.

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

## CLI + App Integration

| Test ID | Description |
|---------|-------------|
| I-01 | Export from app → upload with CLI → pedal reflects app-authored profiles |
| I-02 | Edit profiles.json in text editor → import into app → correct profiles; no validation errors |
| I-03 | App uploads profiles → CLI validate same file → exits 0 |
| I-04 | CLI uploads → app imports same file → JSON Preview matches uploaded file |

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

- [ ] E2E-01–E2E-04 all completed and pass criteria met
- [ ] Usability matrix (Part 2.9) filled in; UX improvement tasks filed for any criterion ≤ 3
- [ ] I-01–I-04 all pass
- [ ] R-01–R-07 all pass
- [ ] Any failures filed as child tasks using the defect template in FEATURE_TEST_PLAN.md
