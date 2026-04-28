---
id: TASK-109
title: On-Device Multi-Press Integration Test (ESP32)
status: closed
closed: 2026-04-24
opened: 2026-04-17
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
epic: LongDoublePress
order: 6
prerequisites: [TASK-104]
---

## Description

Create a Unity on-device test suite for `Button` long-press and double-press detection running on real ESP32 hardware. Tests simulate button presses by directly manipulating the ISR and `millis()` context, and assert that the correct event flags are set.

## Acceptance Criteria

- [x] `test/test_multipress_esp32/` directory created with a Unity test suite
- [x] Test 1 — short press: ISR triggered, release before 500 ms → only `event()` fires, `holdDurationMs()` returns 0 after release
- [x] Test 2 — long press: ISR triggered, release after 600 ms → `holdDurationMs() >= 500` during hold; flag set before release
- [x] Test 3 — double press: two presses within 300 ms → `doublePressEvent()` returns `true`; `event()` does NOT also fire
- [x] `make test-esp32-multipress` target added to `Makefile`
- [x] Test requires: ESP32 (NodeMCU-32S) with button wired to a GPIO matching `builder_config.h`

## Files to Touch

- `test/test_multipress_esp32/` (new directory)
- `test/test_multipress_esp32/test_main.cpp` (new)
- `Makefile` — add `test-esp32-multipress` target

## Test Plan

**On-device tests** (`make test-esp32-multipress`):

- Requires ESP32 (NodeMCU-32S) connected via USB

## Prerequisites

- **TASK-104** — `Button` timing extensions must be in place before the on-device test can exercise them

## Notes

This task requires physical hardware. Mark as `[hardware-required]` in any CI documentation. Can be developed in parallel with TASK-105 through TASK-108 since it only depends on TASK-104.

## Blocking issue — double press not detectable manually (2026-04-23)

Test 3 (`test_double_press_suppresses_single`) could not be verified by hand. The debounce window is 100 ms and the double-press window is 300 ms (widened to 500 ms in the test), leaving a ~200–400 ms sweet spot that is too tight to hit reliably by hand.

Tests 1 (short press) and 2 (long hold) pass. Test 3 is blocked until an automated hardware test bed is available that can simulate precise button timings programmatically. Revisit this test once that infrastructure exists.

## Re-evaluation (2026-04-24) — the failure was a real bug, not timing

On closer inspection the failure was not caused by human timing. The `Button` implementation had two independent bugs that made Test 3 fail deterministically regardless of how well-timed the taps were:

1. **`event()` fired on the first press before the second arrived.** The main loop (and the on-device test) polls `event()` every iteration — as soon as the first falling edge set `pressCount=1`, `event()` returned true and the single-press action was dispatched before the second press had a chance to land. In production this meant every double-press emitted the single-press action first, then the double-press action.
2. **`singlePressSuppressed_` was a bool that suppressed only one of the two queued presses.** Even if `event()` had been drained only after both presses, the second drained call still returned true and leaked a single press.

Both bugs are now covered by host tests (`DoublePressEvent_SingleNotLeakedWhenPolledBetweenPresses`, `DoublePressEvent_BothQueuedPressesSuppressed`) in [test/unit/test_button.cpp](../../../../test/unit/test_button.cpp) and fixed in [lib/hardware/esp32/src/button.cpp](../../../../lib/hardware/esp32/src/button.cpp):

- On a double-press ISR, `pressCount` is cleared so neither tap leaks.
- `event()` defers reporting by up to `doublePressWindow_` (default 300 ms) so a second press can pre-empt the single.

With the fix in place, the existing on-device Test 3 should pass when performed at the ~500 ms sweet spot. Re-run `/test-device esp32-multipress` on hardware to confirm before closing this task.

## Hardware confirmation (2026-04-24)

All three on-device tests passed on the connected ESP32 (NodeMCU-32S): short press, long hold, and double-press suppression. The `Button` fix is verified end-to-end on real hardware.
